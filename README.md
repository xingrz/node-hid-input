node-hid-input
==========

[![][npm-version]][npm-url] [![][npm-downloads]][npm-url] [![license][license-img]][license-url] [![issues][issues-img]][issues-url] [![stars][stars-img]][stars-url] [![commits][commits-img]][commits-url]

Node.js binding for reading text inputs on Linux, such as barcode scanners.

### Why another...?

Features:

- [x] First class TypeScript support
- [x] Node-API addon for better parsing of `input_event`
- [x] Supports exclusive (`EVIOCGRAB`) open
- [x] Mocking interface for better unit testing, even on platforms other than Linux

## Installation

```sh
npm install --save hid-input
```

### Build from source

```sh
npm install --save hid-input --build-from-source
```

Additional build dependencies are required on different platforms:

#### Debian/Ubuntu

```sh
sudo apt install -y build-essential cmake ninja-build
```

#### macOS

```sh
brew install cmake ninja
```

## Usage

```ts
import { createInput, listInputs } from 'hid-input';

const inputs = await listInputs();
console.log(inputs);

const reader = createInput(inputs[0].path);
reader.once('open', ()=>{
  console.log('device opened');
});
reader.on('data', (input) => {
  console.log(`- scanned: ${input}`);
});
```

## License

[MIT License](LICENSE)

[npm-version]: https://img.shields.io/npm/v/hid-input.svg?style=flat-square
[npm-downloads]: https://img.shields.io/npm/dm/hid-input.svg?style=flat-square
[npm-url]: https://www.npmjs.org/package/hid-input
[license-img]: https://img.shields.io/github/license/xingrz/node-hid-input?style=flat-square
[license-url]: LICENSE
[issues-img]: https://img.shields.io/github/issues/xingrz/node-hid-input?style=flat-square
[issues-url]: https://github.com/xingrz/node-hid-input/issues
[stars-img]: https://img.shields.io/github/stars/xingrz/node-hid-input?style=flat-square
[stars-url]: https://github.com/xingrz/node-hid-input/stargazers
[commits-img]: https://img.shields.io/github/last-commit/xingrz/node-hid-input?style=flat-square
[commits-url]: https://github.com/xingrz/node-hid-input/commits/master
