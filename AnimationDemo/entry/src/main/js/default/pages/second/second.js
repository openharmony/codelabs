import router from '@system.router'

export default {
    data: {
        title: 'World'
    },
    onclick: function () {
        router.replace({
            uri: "pages/index/index"
        })
    }
}
