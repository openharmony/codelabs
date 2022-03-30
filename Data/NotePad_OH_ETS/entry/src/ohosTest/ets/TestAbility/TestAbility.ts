import Ability from '@ohos.application.Ability'
import file from '@system.file'

import {Core, ExpectExtend, OhReport, ReportExtend} from "deccjsunit/index"
import testsuite from "../test/List.test.ets"

export default class TestAbility extends Ability {
    onCreate(want, launchParam) {
        console.log("TestAbility onCreate")
        console.info("start run d.ts testcase!!!!")
        const core = Core.getInstance()
        const expectExtend = new ExpectExtend({
            'id': 'extend'
        })
        core.addService('expect', expectExtend)
        const reportExtend = new ReportExtend(file)
        const ohReport = new OhReport({
            'delegator': 'helloworld'
        })
        core.addService('report', reportExtend)
        core.addService('report', ohReport)
        core.init()
        core.subscribeEvent('task', reportExtend)
        core.subscribeEvent('spec', ohReport)
        core.subscribeEvent('suite', ohReport)
        core.subscribeEvent('task', ohReport)
        const configService = core.getDefaultService("config")
        configService.setConfig(want.parameters)
        testsuite()
        core.execute()
    }

    onDestroy() {
        console.log("TestAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        console.log("TestAbility onWindowStageCreate")
        windowStage.setUIContent(this.context, "pages/index", null)
        globalThis.abilityContext = this.context;
    }

    onWindowStageDestroy() {
        console.log("TestAbility onWindowStageDestroy")
    }

    onForeground() {
        console.log("TestAbility onForeground")
    }

    onBackground() {
        console.log("TestAbility onBackground")
    }
};
