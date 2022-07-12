import { createInput, createMockInput, listInputs } from './lib/index.js';

const args = process.argv.slice(2);
const mock = args.includes('--mock') || args.includes('-m');
const exclusive = args.includes('--exclusive') || args.includes('-e');
const path = args.find(arg => !arg.startsWith('-'));

if (!path) {
  console.log(`Usage: npm run demo -- [options] <path>`);
  console.log('');
  console.log('Options:');
  console.log('  -m, --mock');
  console.log('    Mock');
  console.log('  -e, --exclusive');
  console.log('    Exclusive mode');
  process.exit(1);
}

const input = (mock ? createMockInput : createInput)(path, { exclusive });

input.once('open', () => {
  console.log('-- Input opened --');
});

input.once('close', () => {
  console.log('== Input closed ==');
  process.exit();
});

input.once('error', (e) => {
  console.error('!! Input error !!');
  console.error(e);
});

input.on('data', (input) => {
  console.log({ input });
});

process.once('SIGINT', () => {
  console.log('-- SIGINT --');
  input.destroy();
});

process.nextTick(async () => {
  console.log(await listInputs());
});
