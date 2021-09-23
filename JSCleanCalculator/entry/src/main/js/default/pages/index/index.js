import * as cal from '../../common/utils/utils.js'
export default {
  data: {
    CalDisplay: '0',
    pseudoStack: [],
    isResult: false,
    maxSize: 14
  },
  onInit() {
  },
  touchstartfunc(touches) {
    const currentVal = touches.currentTarget.attr.value;
    switch (currentVal) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        this.numberhandle(currentVal);
        break;
      case '+':
      case '÷':
      case '%':
      case '×':
      case '-':
        this.calculatehandle(currentVal);
        break;
      case '.':
        this.pointhandle(currentVal);
        break;
      case 'AC':
      case 'CE':
        this.deletehandle(currentVal);
        break;
      case '=':
        this.resulthandle();
        break;
    }
  },
  numberhandle(param) {
    if (this.CalDisplay === '错误') {
      this.dataInit()
    }
    if (param === '0' && this.CalDisplay === '0') return
    const temp = this.pseudoStack.pop();
    if (this.isResult) {
      this.CalDisplay = param;
      this.pseudoStack.push(this.CalDisplay);
      this.isResult = false;
    } else {
      if (temp && this.isOperator(temp)) {
        this.pseudoStack.push(temp);
        this.CalDisplay = param;
        this.pseudoStack.push(this.CalDisplay);
      } else {
        if (this.CalDisplay.length > this.maxSize){
          this.pseudoStack.push(temp);
          return
        }
        this.CalDisplay = this.CalDisplay === '0' ? param : this.CalDisplay + param;
        this.pseudoStack.push(this.CalDisplay);
      }
    }
  },
  calculatehandle(param) {
    if (this.CalDisplay === '错误') return
    const temp = this.pseudoStack.pop();
    if (temp && this.isOperator(temp)) {
      this.pseudoStack.push(param);
    } else {
      this.pseudoStack.push(temp);
      this.pseudoStack.push(param);
    }
    this.isResult = false;
  },
  pointhandle(param) {
    if (this.CalDisplay === '错误') {
      this.dataInit()
    }
    if (this.isResult) {
      this.CalDisplay = '0' + param;
      this.pseudoStack.push(this.CalDisplay);
      this.isResult = false;
      return;
    }
    if (this.pseudoStack.length === 0) {
      this.CalDisplay = '0' + param;
      this.pseudoStack.push(this.CalDisplay);
    } else {
      const temp = this.pseudoStack.pop();
      if (temp && this.isOperator(temp)) {
        this.pseudoStack.push(temp);
        this.CalDisplay = '0' + param;
        this.pseudoStack.push(this.CalDisplay);
      } else {
        if (this.CalDisplay.indexOf(param) !== -1) {
          return;
        }
        this.CalDisplay = this.CalDisplay === '0' ? '0' + param : this.CalDisplay + param;
        this.pseudoStack.push(this.CalDisplay);
      }
    }
  },
  deletehandle(param) {
    if (this.isResult || this.CalDisplay === '错误') {
      this.dataInit();
    }
    if (param === 'AC') {
      this.dataInit();
    } else {
      if (this.pseudoStack.length === 0) {
        return;
      }
      const temp = this.pseudoStack.pop();
      if (temp && this.isOperator(temp)) {
        return;
      }
      const result = temp.substring(0, temp.length - 1);
      if (result.length === 0) {
        this.CalDisplay = '0';
        return;
      }
      this.CalDisplay = result;
      this.pseudoStack.push(result);
    }
  },
  resulthandle() {
    if (this.CalDisplay === '错误') return
    const pseudoStackLength = this.pseudoStack.length;
    if (pseudoStackLength < 3) {
      return;
    }
    const secondParameter = Number(this.pseudoStack.pop());
    const operator = this.pseudoStack.pop();
    const firstParameter = Number(this.pseudoStack.pop());
    let result;
    switch (operator) {
      case '+':
        result = cal.accAdd(firstParameter, secondParameter);
        break;
      case '÷':
        result = cal.accDiv(firstParameter, secondParameter);
        break;
      case '%':
        result = firstParameter % secondParameter;
        break;
      case '×':
        result = cal.accMul(firstParameter, secondParameter);
        break;
      case '-':
        result = cal.accSub(firstParameter, secondParameter);
        break;
    }
    if (Math.abs(result) === Infinity || isNaN(result)) {
        result = '错误'
    }
    if (result.toString().length > 14) {
        result = Number(result).toExponential([6])
    }
    result = result.toString();
    this.CalDisplay = result;
    this.pseudoStack.push(result);
    this.isResult = true;
  },
  isNumber(param) {
    const numEnums = '1234567890';
    return numEnums.indexOf(param) !== -1;
  },
  isOperator(param) {
    const operator = '+÷%×-';
    return operator.indexOf(param) !== -1;
  },
  dataInit() {
    this.CalDisplay = '0';
    this.pseudoStack = [];
    this.isResult = false;
  }
};
