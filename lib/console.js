'use strict';

function Console(stdout, stderr) {
  if (!(this instanceof Console)) {
      return new Console(stdout, stderr);
  }
  this._stdout = stdout || print;
  this._stderr = stderr || print_error;
}

Console.prototype.log = function () {
  const args = Array.from(arguments);
  this._stdout(...args);
}

Console.prototype.error = function () {
  const args = Array.from(arguments);
  this._stderr(...args);
}

this.console = new Console();