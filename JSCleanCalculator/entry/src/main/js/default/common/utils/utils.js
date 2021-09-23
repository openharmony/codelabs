/**
* 加法
* 计算方式：1.先将所有的小数乘为整数；
          2.待加减运算执行完之后再除去对应的 m 的值，将其变为小数输出;
* */
function accAdd(arg1, arg2) {
  let r1, r2;
  try {
    r1 = arg1.toString().split('.')[1].length;
  } catch (e) {
    r1 = 0;
  }
  try {
    r2 = arg2.toString().split('.')[1].length;
  } catch (e) {
    r2 = 0;
  }
  const m = Math.pow(10, Math.max(r1, r2));
  return (arg1 * m + arg2 * m) / m;
}

/**
* 减法
**/
function accSub(arg1, arg2) {
  return accAdd(arg1, -arg2);
}
/**
* 乘法
* */
function accMul(arg1, arg2) {
  let m = 0; const s1 = arg1.toString(); const s2 = arg2.toString();
  try {
    m += s1.split('.')[1].length;
  } catch (e) {
    m += 0;
  }
  try {
    m += s2.split('.')[1].length;
  } catch (e) {
    m += 0;
  }
  return Number(s1.replace('.', '')) * Number(s2.replace('.', '')) / Math.pow(10, m);
}

/**
* 除法
* */
function accDiv(arg1, arg2) {
  let t1 = 0; let t2 = 0;
  try {
    t1 = arg1.toString().split('.')[1].length;
  } catch (e) {
    t1 = 0;
  }
  try {
    t2 = arg2.toString().split('.')[1].length;
  } catch (e) {
    t2 = 0;
  }
  const r1 = Number(arg1.toString().replace('.', ''));
  const r2 = Number(arg2.toString().replace('.', ''));
  return r1 / r2 * Math.pow(10, t2 - t1);
}

export {accAdd, accSub, accMul, accDiv};

