import { join, resolve } from 'path';

export interface Input {
  read(): Promise<string>;
  close(): Promise<void>;
}

export interface Binding {
  openInput?: BindingFactory;
}

export interface BindingFactory {
  (path: string, options: BindingOpenOptions): Promise<Input>;
}

export interface BindingOpenOptions {
  exclusive?: boolean;
}

export default ((name: string): Binding => {
  const moduleRoot = resolve(__dirname, '..');
  const paths = [
    join(moduleRoot, 'build', 'Release', name),
    join(moduleRoot, 'prebuilds', `${process.platform}-${process.arch}`, name)
  ];

  for (const path of paths) {
    try {
      return require(path);
    } catch (e) {
    }
  }

  throw new Error('Failed loading addon');
})('hid-input.node');
