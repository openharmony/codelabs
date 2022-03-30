import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import file from '@system.file'

export default function abilityTest() {
    describe('ActsAbilityTest', function () {
        it('String_assertContain_success',0, function () {
            let a = 'abc'
            let b = 'b'
            expect(a).assertContain(b)
        })
        it('testCaseCallback', 0, async function (done) {
            file.writeText({
                uri: 'internal://app/test.txt',
                text: 'abc',
                encoding: 'UTF-8',
                success: () => {
                    console.info("成功")
                    done()
                },
                fail:function(data, code) {
                    console.info('失败code:' + code)
                    done()
                }
            })
        })
    })
}