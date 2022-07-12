import { once } from 'events';
import { createServer, Server, Socket } from 'net';
import { createInterface, Interface } from 'readline';
import { Duplex, PassThrough } from 'stream';
import { promisify } from 'util';
import type { BindingFactory, BindingOpenOptions, Input } from "./binding";

export const openMockInput = <BindingFactory>async function (path: string, _options: BindingOpenOptions) {
  const input = new SocketInput();
  await input.open(path);
  return input;
}

class SocketInput implements Input {
  private server: Server;
  private muxer: Duplex;
  private rl: Interface;

  constructor() {
    this.server = createServer(this.onConnection.bind(this));
    this.muxer = new PassThrough();
    this.rl = createInterface({ input: this.muxer, crlfDelay: Infinity });
  }

  onConnection(socket: Socket) {
    socket.pipe(this.muxer);
  }

  async open(path: string): Promise<void> {
    this.server.listen(path);
  }

  async read(): Promise<string> {
    const [line] = await once(this.rl, 'line');
    return line;
  }

  async close(): Promise<void> {
    await promisify(this.server.close.bind(this.server))();
  }
}
