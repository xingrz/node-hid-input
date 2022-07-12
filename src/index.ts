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
