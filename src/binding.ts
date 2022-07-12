export interface Input {
  read(): Promise<string>;
  close(): Promise<void>;
}

export interface BindingFactory {
  (path: string, options: BindingOpenOptions): Promise<Input>;
}

export interface BindingOpenOptions {
  exclusive?: boolean;
}
