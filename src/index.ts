import { constants } from 'fs';
import { access, readdir, readFile, realpath } from 'fs/promises';
import { join } from 'path';
import { Readable } from 'stream';
import binding, { BindingFactory, Input } from './binding';
import { openMockInput } from './socket';

class HidInputStream extends Readable {
  private binding: Input | undefined;
  private reading = false;

  constructor(open: BindingFactory, path: string, options?: InputOpenOptions) {
    super({ objectMode: true });
    process.nextTick(async () => {
      try {
        this.binding = await open(path, {
          exclusive: options?.exclusive || false,
        });
        this.emit('open');
        if (this.reading) {
          this._read(0);
        }
      } catch (e) {
        this.emit('error', e);
      }
    });
  }

  override _read(_size: number): void {
    process.nextTick(async () => {
      if (!this.binding) {
        this.reading = true;
        return;
      }

      try {
        const data = await this.binding.read();
        this.push(data);
      } catch (e) {
        this.emit('error', e);
      }
    });
  }

  override _destroy(_error: Error | null, callback: (error?: Error | null | undefined) => void): void {
    process.nextTick(async () => {
      await this.binding?.close();
      callback();
    });
  }
}

export interface InputOpenOptions {
  exclusive?: boolean;
}

export function createInput(path: string, options?: InputOpenOptions) {
  if (binding.openInput) {
    return new HidInputStream(binding.openInput, path, options);
  } else {
    throw new Error(`createInput is not supported on ${process.platform}`);
  }
}

export function createMockInput(path: string, options?: InputOpenOptions) {
  return new HidInputStream(openMockInput, path, options);
}

export interface InputDevice {
  path: string;
  name: string;
}

const INPUT_CLASS_PATH = '/sys/class/input';

export async function listInputs(): Promise<InputDevice[]> {
  try {
    await access(INPUT_CLASS_PATH, constants.F_OK);
  } catch (e) {
    return [];
  }

  const inputs: InputDevice[] = [];

  for (const inputId of await readdir(INPUT_CLASS_PATH)) {
    if (!inputId.startsWith('input')) continue;
    const inputNode = join(INPUT_CLASS_PATH, inputId);

    try {
      const subsystem = await realpath(join(inputNode, 'device', 'subsystem'));
      if (subsystem != '/sys/bus/hid') continue;

      const eventId = (await readdir(inputNode)).find((name) => name.startsWith('event'));
      if (!eventId) continue;

      const name = (await readFile(join(inputNode, 'name'), 'utf-8')).trim();
      if (!name) continue;

      inputs.push({ name, path: `/dev/input/${eventId}` });
    } catch (ignored) {
    }
  }

  return inputs;
}
