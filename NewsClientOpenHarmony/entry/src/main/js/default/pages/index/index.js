import {titles, newsData} from '../../../default/common/data/data.js';
import router from '@system.router';

export default {
    data: {
        titleList: titles,
        newsList: newsData
    },
    changeNewsType: function (e) {
        var type = titles[e.index].name;
        this.newsList = [];
        if (type === "All") {
            this.newsList = newsData;
        } else {
            var newsArray = [];
            for (var news of newsData) {
                if (news.type === type) {
                    newsArray.push(news);
                }
            }
            this.newsList = newsArray;
        }
    },
    itemClick(news) {
        router.push({
            uri: "pages/detail/detail",
            params: {
                "title": news.title,
                "type": news.type,
                "imgUrl": news.imgUrl,
                "reads": news.reads,
                "likes": news.likes,
                "content": news.content,
            }
        });

    }
}
