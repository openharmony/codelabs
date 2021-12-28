/******/ (() => { // webpackBootstrap
/******/ 	var __webpack_modules__ = ({

/***/ "./lib/json.js!./lib/style.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.css":
/*!*****************************************************************************************************************************************!*\
  !*** ./lib/json.js!./lib/style.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.css ***!
  \*****************************************************************************************************************************************/
/***/ ((module) => {

module.exports = {
  ".container": {
    "display": "flex",
    "flexDirection": "column",
    "justifyContent": "center",
    "alignItems": "center",
    "left": "0px",
    "top": "0px",
    "width": "100%",
    "height": "100%"
  },
  ".title": {
    "fontSize": "60px",
    "textAlign": "center",
    "width": "100%",
    "height": "40%",
    "marginTop": "10px",
    "marginRight": "10px",
    "marginBottom": "10px",
    "marginLeft": "10px"
  },
  ".btn": {
    "width": "50%",
    "height": "100px",
    "fontSize": "40px"
  }
}

/***/ }),

/***/ "./lib/json.js!./lib/template.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.hml":
/*!********************************************************************************************************************************************!*\
  !*** ./lib/json.js!./lib/template.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.hml ***!
  \********************************************************************************************************************************************/
/***/ ((module) => {

module.exports = {
  "attr": {
    "debugLine": "pages/second/second:1",
    "className": "container"
  },
  "type": "div",
  "classList": [
    "container"
  ],
  "children": [
    {
      "attr": {
        "debugLine": "pages/second/second:2",
        "className": "title",
        "value": function () {return this.$t('strings.page')}
      },
      "type": "text",
      "classList": [
        "title"
      ]
    },
    {
      "attr": {
        "debugLine": "pages/second/second:5",
        "className": "btn",
        "type": "button",
        "value": function () {return this.$t('strings.back')}
      },
      "type": "input",
      "classList": [
        "btn"
      ],
      "onBubbleEvents": {
        "click": "onclick"
      }
    }
  ]
}

/***/ }),

/***/ "./node_modules/@babel/runtime/helpers/interopRequireDefault.js":
/*!**********************************************************************!*\
  !*** ./node_modules/@babel/runtime/helpers/interopRequireDefault.js ***!
  \**********************************************************************/
/***/ ((module) => {

"use strict";


function _interopRequireDefault(obj) {
  return obj && obj.__esModule ? obj : {
    "default": obj
  };
}

module.exports = _interopRequireDefault;
module.exports["default"] = module.exports, module.exports.__esModule = true;

function requireModule(moduleName) {
  const systemList = ['system.router', 'system.app', 'system.prompt', 'system.configuration',
  'system.image', 'system.device', 'system.mediaquery', 'ohos.animator', 'system.grid', 'system.resource']
  var target = ''
  if (systemList.includes(moduleName.replace('@', ''))) {
    target = $app_require$('@app-module/' + moduleName.substring(1));
    return target;
  }
  var shortName = moduleName.replace(/@[^.]+.([^.]+)/, '$1');
  if (typeof ohosplugin !== 'undefined' && /@ohos/.test(moduleName)) {
    target = ohosplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  if (typeof systemplugin !== 'undefined') {
    target = systemplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  target = requireNapi(shortName);
  return target;
}


/***/ }),

/***/ "./lib/script.js!./node_modules/babel-loader/lib/index.js?presets[]=D:\\MY\\Hi3516D\\SDK\\version-Daily_Version-20210926_102043-ohos-sdk-LTS\\windows\\js\\3.0.0.0\\build-tools\\ace-loader\\node_modules\\@babel\\preset-env&plugins[]=D:\\MY\\Hi3516D\\SDK\\version-Daily_Version-20210926_102043-ohos-sdk-LTS\\windows\\js\\3.0.0.0\\build-tools\\ace-loader\\node_modules\\@babel\\plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.js":
/*!*************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************!*\
  !*** ./lib/script.js!./node_modules/babel-loader/lib/index.js?presets[]=D:\MY\Hi3516D\SDK\version-Daily_Version-20210926_102043-ohos-sdk-LTS\windows\js\3.0.0.0\build-tools\ace-loader\node_modules\@babel\preset-env&plugins[]=D:\MY\Hi3516D\SDK\version-Daily_Version-20210926_102043-ohos-sdk-LTS\windows\js\3.0.0.0\build-tools\ace-loader\node_modules\@babel\plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.js ***!
  \*************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

module.exports = function(module, exports, $app_require$){"use strict";

var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "./node_modules/@babel/runtime/helpers/interopRequireDefault.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _system = _interopRequireDefault(requireModule("@system.router"));

var _default = {
  data: {
    title: 'World'
  },
  onclick: function onclick() {
    _system["default"].replace({
      uri: "pages/index/index"
    });
  }
};
exports["default"] = _default;

function requireModule(moduleName) {
  const systemList = ['system.router', 'system.app', 'system.prompt', 'system.configuration',
  'system.image', 'system.device', 'system.mediaquery', 'ohos.animator', 'system.grid', 'system.resource']
  var target = ''
  if (systemList.includes(moduleName.replace('@', ''))) {
    target = $app_require$('@app-module/' + moduleName.substring(1));
    return target;
  }
  var shortName = moduleName.replace(/@[^.]+.([^.]+)/, '$1');
  if (typeof ohosplugin !== 'undefined' && /@ohos/.test(moduleName)) {
    target = ohosplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  if (typeof systemplugin !== 'undefined') {
    target = systemplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  target = requireNapi(shortName);
  return target;
}

var moduleOwn = exports.default || module.exports;
var accessors = ['public', 'protected', 'private'];
if (moduleOwn.data && accessors.some(function (acc) {
    return moduleOwn[acc];
  })) {
  throw new Error('For VM objects, attribute data must not coexist with public, protected, or private. Please replace data with public.');
} else if (!moduleOwn.data) {
  moduleOwn.data = {};
  moduleOwn._descriptor = {};
  accessors.forEach(function(acc) {
    var accType = typeof moduleOwn[acc];
    if (accType === 'object') {
      moduleOwn.data = Object.assign(moduleOwn.data, moduleOwn[acc]);
      for (var name in moduleOwn[acc]) {
        moduleOwn._descriptor[name] = {access : acc};
      }
    } else if (accType === 'function') {
      console.warn('For VM objects, attribute ' + acc + ' value must not be a function. Change the value to an object.');
    }
  });
}}
/* generated by ace-loader */


/***/ })

/******/ 	});
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		var cachedModule = __webpack_module_cache__[moduleId];
/******/ 		if (cachedModule !== undefined) {
/******/ 			return cachedModule.exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId](module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
var __webpack_exports__ = {};
// This entry need to be wrapped in an IIFE because it need to be isolated against other modules in the chunk.
(() => {
/*!******************************************************************************************************************!*\
  !*** ../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.hml?entry ***!
  \******************************************************************************************************************/
var $app_template$ = __webpack_require__(/*! !!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/lib/json.js!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/lib/template.js!./second.hml */ "./lib/json.js!./lib/template.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.hml")
var $app_style$ = __webpack_require__(/*! !!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/lib/json.js!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/lib/style.js!./second.css */ "./lib/json.js!./lib/style.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.css")
var $app_script$ = __webpack_require__(/*! !!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/lib/script.js!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/node_modules/babel-loader?presets[]=D:\MY\Hi3516D\SDK\version-Daily_Version-20210926_102043-ohos-sdk-LTS\windows\js\3.0.0.0\build-tools\ace-loader\node_modules\@babel\preset-env&plugins[]=D:\MY\Hi3516D\SDK\version-Daily_Version-20210926_102043-ohos-sdk-LTS\windows\js\3.0.0.0\build-tools\ace-loader\node_modules\@babel\plugin-transform-modules-commonjs&comments=false!../../../../../../../../../../SDK/version-Daily_Version-20210926_102043-ohos-sdk-LTS/windows/js/3.0.0.0/build-tools/ace-loader/lib/resource-reference-script.js!./second.js */ "./lib/script.js!./node_modules/babel-loader/lib/index.js?presets[]=D:\\MY\\Hi3516D\\SDK\\version-Daily_Version-20210926_102043-ohos-sdk-LTS\\windows\\js\\3.0.0.0\\build-tools\\ace-loader\\node_modules\\@babel\\preset-env&plugins[]=D:\\MY\\Hi3516D\\SDK\\version-Daily_Version-20210926_102043-ohos-sdk-LTS\\windows\\js\\3.0.0.0\\build-tools\\ace-loader\\node_modules\\@babel\\plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!../../../../../../../CodeLab/20210929/NewsDemo/entry/src/main/js/MainAbility/pages/second/second.js")

$app_define$('@app-component/second', [], function($app_require$, $app_exports$, $app_module$) {

$app_script$($app_module$, $app_exports$, $app_require$)
if ($app_exports$.__esModule && $app_exports$.default) {
$app_module$.exports = $app_exports$.default
}

$app_module$.exports.template = $app_template$

$app_module$.exports.style = $app_style$

})
$app_bootstrap$('@app-component/second',undefined,undefined)
})();

/******/ })()
;