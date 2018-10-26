const obj = {};
obj.a = 1;
obj.b = true;
obj.c = "c";
obj.d = 1.1;
console.error(1);
Object.keys(obj).map(key => {
  console.log(key, obj[key]);
});
console.log();

throw new TypeError("测试错误");