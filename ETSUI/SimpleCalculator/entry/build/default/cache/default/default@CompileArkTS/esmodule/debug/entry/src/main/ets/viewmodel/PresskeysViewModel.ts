import { PressKeysItem } from "@bundle:com.example.simplecalculator/entry/ets/viewmodel/PressKeysItem";
export class PressKeysViewModel {
    /**
     * Key array data.
     */
    getPressKeys(): Array<Array<PressKeysItem>> {
        return [
            [
                new PressKeysItem(0, '32vp', '32vp', 'clean', { "id": 16777246, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }),
                new PressKeysItem(1, '19vp', '43vp', '7'),
                new PressKeysItem(1, '19vp', '43vp', '4'),
                new PressKeysItem(1, '19vp', '43vp', '1'),
                new PressKeysItem(1, '25vp', '43vp', '%')
                // {
                //   flag: 0,
                //   width: '32vp',
                //   height: '32vp',
                //   value: 'clean',
                //   source: $r('app.media.ic_clean')
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '7'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '4'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '1'
                // },
                // {
                //   flag: 1,
                //   width: '25vp',
                //   height: '43vp',
                //   value: '%'
                // }
            ],
            [
                new PressKeysItem(0, '32vp', '32vp', 'div', { "id": 16777248, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }),
                new PressKeysItem(1, '19vp', '43vp', '8'),
                new PressKeysItem(1, '19vp', '43vp', '5'),
                new PressKeysItem(1, '19vp', '43vp', '2'),
                new PressKeysItem(1, '19vp', '43vp', '0')
                // {
                //   flag: 0,
                //   width: '32vp',
                //   height: '32vp',
                //   value: 'div',
                //   source: $r('app.media.ic_div')
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '8'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '5'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '2'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '0'
                // }
            ],
            [
                new PressKeysItem(0, '32vp', '32vp', 'mul', { "id": 16777251, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }),
                new PressKeysItem(1, '19vp', '43vp', '9'),
                new PressKeysItem(1, '19vp', '43vp', '6'),
                new PressKeysItem(1, '19vp', '43vp', '3'),
                new PressKeysItem(1, '19vp', '43vp', '.')
                // {
                //   flag: 0,
                //   width: '32vp',
                //   height: '32vp',
                //   value: 'mul',
                //   source: $r('app.media.ic_mul')
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '9'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '6'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '3'
                // },
                // {
                //   flag: 1,
                //   width: '19vp',
                //   height: '43vp',
                //   value: '.'
                // }
            ],
            [
                new PressKeysItem(0, '30.48vp', '20vp', 'del', { "id": 16777247, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }),
                new PressKeysItem(0, '24vp', '24vp', 'min', { "id": 16777250, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }),
                new PressKeysItem(0, '32vp', '32vp', 'add', { "id": 16777245, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }),
                new PressKeysItem(0, '32vp', '32vp', 'equ', { "id": 16777249, "type": 20000, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" })
                // {
                //   flag: 0,
                //   width: '30.48vp',
                //   height: '20vp',
                //   value: 'del',
                //   source: $r('app.media.ic_del')
                // },
                // {
                //   flag: 0,
                //   width: '24vp',
                //   height: '24vp',
                //   value: 'min',
                //   source: $r('app.media.ic_min')
                // },
                // {
                //   flag: 0,
                //   width: '32vp',
                //   height: '32vp',
                //   value: 'add',
                //   source: $r('app.media.ic_add')
                // },
                // {
                //   flag: 0,
                //   width: '32vp',
                //   height: '32vp',
                //   value: 'equ',
                //   source: $r('app.media.ic_equ')
                // }
            ]
        ];
    }
}
let keysModel = new PressKeysViewModel();
export default keysModel as PressKeysViewModel;
