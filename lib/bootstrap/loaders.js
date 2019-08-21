'use strict';

(function bootstrapLoaders(process, getBinding){
  const loaderId = 'bootstrap/loaders';
  const moduleLoadList = [];

  Object.defineProperty(process, 'moduleLoadList', {
    value: moduleLoadList,
    configurable: true,
    enumerable: true,
    writable: false
  });

  {
    const bindingObj = Object.create(null);
    process.binding = function binding(module) {
      module = String(module);
      let mod = bindingObj[module];
      if (typeof mod !== 'object') {
        mod = bindingObj[module] = getBinding(module);
        moduleLoadList.push(`Binding ${module}`);
      }
      return mod;
    };
  }

  const contextify = process.binding('contextify');

  function NativeModule(id) {
    this.filename = `${id}.js`;
    this.id = id;
    this.exports = {};
    this.reflect = undefined;
    this.exportKeys = undefined;
    this.loaded = false;
    this.loading = false;
    this.script = null;
  };

  NativeModule._cache = {};
  NativeModule._source = getBinding("natives");
  NativeModule.wrapper = [
    '(function (exports, require, module, process) {',
    '\n});'
  ];

  NativeModule.require = function(id) {
    if (id === loaderId) return loaderExports;
    const cached = NativeModule.getCached(id);
    if (cached && (cached.loaded || cached.loading)) return cached.exports; 

    if (!NativeModule.exists(id)) {
      const err = new Error(`No such built-in module: ${id}`);
      err.code = 'ERR_UNKNOWN_BUILTIN_MODULE';
      err.name = 'Error [ERR_UNKNOWN_BUILTIN_MODULE]';
      throw err;
    }

    moduleLoadList.push(`NativeModule ${id}`);

    const nativeModule = new NativeModule(id);

    nativeModule.cache();
    nativeModule.compile();

    return nativeModule.exports;
  };

  NativeModule.exists = function(id) {
    return NativeModule._source.hasOwnProperty(id);
  };

  NativeModule.getSource = function (id) {
    return NativeModule._source[id];
  };

  NativeModule.getCached = function(id) {
    return NativeModule._cache[id];
  };

  NativeModule.wrap = function(script) {
    return NativeModule.wrapper[0] + script + NativeModule.wrapper[1];
  };



  NativeModule.prototype.cache = function() {
    NativeModule._cache[this.id] = this;
  };

  NativeModule.prototype.compile = function() {
    let source = NativeModule.getSource(this.id);
    source = NativeModule.wrap(source);

    this.loading = true;
    try {
      // TODO
      this.loaded = true;
    } finally {
      this.loading = false;
    }
  };

  const loaderExports = {NativeModule};

  return loaderExports;
});