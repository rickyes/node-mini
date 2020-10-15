'use strict';

async function hello() {
  const text = await Promise.resolve('Hello hyj1991.');
  console.log(text);
}

hello();
