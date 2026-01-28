export interface City {
  name: string;
  code: string;
}

export interface Province {
  name: string;
  cities: City[];
}

export const WORLD_DATA: Record<string, Province[]> = {
  "中国": [
    {
      name: "北京市",
      cities: [
        {
          name: "东城区",
          code: "110101"
        },
        {
          name: "西城区",
          code: "110102"
        },
        {
          name: "朝阳区",
          code: "110105"
        },
        {
          name: "丰台区",
          code: "110106"
        },
        {
          name: "石景山区",
          code: "110107"
        },
        {
          name: "海淀区",
          code: "110108"
        },
        {
          name: "门头沟区",
          code: "110109"
        },
        {
          name: "房山区",
          code: "110111"
        },
        {
          name: "通州区",
          code: "110112"
        },
        {
          name: "顺义区",
          code: "110113"
        },
        {
          name: "昌平区",
          code: "110114"
        },
        {
          name: "大兴区",
          code: "110115"
        },
        {
          name: "怀柔区",
          code: "110116"
        },
        {
          name: "平谷区",
          code: "110117"
        },
        {
          name: "密云区",
          code: "110118"
        },
        {
          name: "延庆区",
          code: "110119"
        }
      ]
    },
    {
      name: "上海市",
      cities: [
        {
          name: "黄浦区",
          code: "310101"
        },
        {
          name: "徐汇区",
          code: "310104"
        },
        {
          name: "长宁区",
          code: "310105"
        },
        {
          name: "静安区",
          code: "310106"
        },
        {
          name: "普陀区",
          code: "310107"
        },
        {
          name: "虹口区",
          code: "310108"
        },
        {
          name: "杨浦区",
          code: "310110"
        },
        {
          name: "闵行区",
          code: "310112"
        },
        {
          name: "宝山区",
          code: "310113"
        },
        {
          name: "嘉定区",
          code: "310114"
        },
        {
          name: "浦东新区",
          code: "310115"
        },
        {
          name: "金山区",
          code: "310116"
        },
        {
          name: "松江区",
          code: "310117"
        },
        {
          name: "青浦区",
          code: "310118"
        },
        {
          name: "奉贤区",
          code: "310120"
        },
        {
          name: "崇明区",
          code: "310151"
        }
      ]
    },
    {
      name: "天津市",
      cities: [
        {
          name: "和平区",
          code: "120101"
        },
        {
          name: "河东区",
          code: "120102"
        },
        {
          name: "河西区",
          code: "120103"
        },
        {
          name: "南开区",
          code: "120104"
        },
        {
          name: "河北区",
          code: "120105"
        },
        {
          name: "红桥区",
          code: "120106"
        },
        {
          name: "东丽区",
          code: "120110"
        },
        {
          name: "西青区",
          code: "120111"
        },
        {
          name: "津南区",
          code: "120112"
        },
        {
          name: "北辰区",
          code: "120113"
        },
        {
          name: "武清区",
          code: "120114"
        },
        {
          name: "宝坻区",
          code: "120115"
        },
        {
          name: "滨海新区",
          code: "120116"
        },
        {
          name: "宁河区",
          code: "120117"
        },
        {
          name: "静海区",
          code: "120118"
        },
        {
          name: "蓟州区",
          code: "120119"
        }
      ]
    },
    {
      name: "重庆市",
      cities: [
        {
          name: "万州区",
          code: "500101"
        },
        {
          name: "涪陵区",
          code: "500102"
        },
        {
          name: "渝中区",
          code: "500103"
        },
        {
          name: "大渡口区",
          code: "500104"
        },
        {
          name: "江北区",
          code: "500105"
        },
        {
          name: "沙坪坝区",
          code: "500106"
        },
        {
          name: "九龙坡区",
          code: "500107"
        },
        {
          name: "南岸区",
          code: "500108"
        },
        {
          name: "北碚区",
          code: "500109"
        },
        {
          name: "渝北区",
          code: "500112"
        },
        {
          name: "巴南区",
          code: "500113"
        },
        {
          name: "长寿区",
          code: "500115"
        },
        {
          name: "江津区",
          code: "500116"
        },
        {
          name: "合川区",
          code: "500117"
        },
        {
          name: "永川区",
          code: "500118"
        },
        {
          name: "南川区",
          code: "500119"
        },
        {
          name: "璧山区",
          code: "500120"
        },
        {
          name: "铜梁区",
          code: "500151"
        }
      ]
    },
    {
      name: "广东省",
      cities: [
        {
          name: "广州市",
          code: "440100"
        },
        {
          name: "韶关市",
          code: "440200"
        },
        {
          name: "深圳市",
          code: "440300"
        },
        {
          name: "珠海市",
          code: "440400"
        },
        {
          name: "汕头市",
          code: "440500"
        },
        {
          name: "佛山市",
          code: "440600"
        },
        {
          name: "江门市",
          code: "440700"
        },
        {
          name: "湛江市",
          code: "440800"
        },
        {
          name: "茂名市",
          code: "440900"
        },
        {
          name: "肇庆市",
          code: "441200"
        },
        {
          name: "惠州市",
          code: "441300"
        },
        {
          name: "梅州市",
          code: "441400"
        },
        {
          name: "汕尾市",
          code: "441500"
        },
        {
          name: "河源市",
          code: "441600"
        },
        {
          name: "阳江市",
          code: "441700"
        },
        {
          name: "清远市",
          code: "441800"
        },
        {
          name: "东莞市",
          code: "441900"
        },
        {
          name: "中山市",
          code: "442000"
        },
        {
          name: "潮州市",
          code: "445100"
        },
        {
          name: "揭阳市",
          code: "445200"
        },
        {
          name: "云浮市",
          code: "445300"
        }
      ]
    },
    {
      name: "江苏省",
      cities: [
        {
          name: "南京市",
          code: "320100"
        },
        {
          name: "无锡市",
          code: "320200"
        },
        {
          name: "徐州市",
          code: "320300"
        },
        {
          name: "常州市",
          code: "320400"
        },
        {
          name: "苏州市",
          code: "320500"
        },
        {
          name: "南通市",
          code: "320600"
        },
        {
          name: "连云港市",
          code: "320700"
        },
        {
          name: "淮安市",
          code: "320800"
        },
        {
          name: "盐城市",
          code: "320900"
        },
        {
          name: "扬州市",
          code: "321000"
        },
        {
          name: "镇江市",
          code: "321100"
        },
        {
          name: "泰州市",
          code: "321200"
        },
        {
          name: "宿迁市",
          code: "321300"
        }
      ]
    },{
    name: "浙江省",
    cities: [
      {
        name: "杭州市",
        code: "330100"
      },
      {
        name: "宁波市",
        code: "330200"
      },
      {
        name: "温州市",
        code: "330300"
      },
      {
        name: "嘉兴市",
        code: "330400"
      },
      {
        name: "湖州市",
        code: "330500"
      },
      {
        name: "绍兴市",
        code: "330600"
      },
      {
        name: "金华市",
        code: "330700"
      },
      {
        name: "衢州市",
        code: "330800"
      },
      {
        name: "舟山市",
        code: "330900"
      },
      {
        name: "台州市",
        code: "331000"
      },
      {
        name: "丽水市",
        code: "331100"
      }
    ]
  },{
    name: "山东省",
    cities: [
      {
        name: "济南市",
        code: "370100"
      },
      {
        name: "青岛市",
        code: "370200"
      },
      {
        name: "淄博市",
        code: "370300"
      },
      {
        name: "枣庄市",
        code: "370400"
      },
      {
        name: "东营市",
        code: "370500"
      },
      {
        name: "烟台市",
        code: "370600"
      },
      {
        name: "潍坊市",
        code: "370700"
      },
      {
        name: "济宁市",
        code: "370800"
      },
      {
        name: "泰安市",
        code: "370900"
      },
      {
        name: "威海市",
        code: "371000"
      },
      {
        name: "日照市",
        code: "371100"
      },
      {
        name: "临沂市",
        code: "371300"
      },
      {
        name: "德州市",
        code: "371400"
      },
      {
        name: "聊城市",
        code: "371500"
      },
      {
        name: "滨州市",
        code: "371600"
      },
      {
        name: "菏泽市",
        code: "371700"
      }
    ]
  },{
    name: "河北省",
    cities: [
      {
        name: "石家庄市",
        code: "130100"
      },
      {
        name: "唐山市",
        code: "130200"
      },
      {
        name: "秦皇岛市",
        code: "130300"
      },
      {
        name: "邯郸市",
        code: "130400"
      },
      {
        name: "邢台市",
        code: "130500"
      },
      {
        name: "保定市",
        code: "130600"
      },
      {
        name: "张家口市",
        code: "130700"
      },
      {
        name: "承德市",
        code: "130800"
      },
      {
        name: "沧州市",
        code: "131000"
      },
      {
        name: "廊坊市",
        code: "131100"
      },
      {
        name: "衡水市",
        code: "131100"
      }
    ]
  },{
    name: "河南省",
    cities: [
      {
        name: "郑州市",
        code: "410100"
      },
      {
        name: "开封市",
        code: "410200"
      },
      {
        name: "洛阳市",
        code: "410300"
      },
      {
        name: "平顶山市",
        code: "410400"
      },
      {
        name: "安阳市",
        code: "410500"
      },
      {
        name: "鹤壁市",
        code: "410600"
      },
      {
        name: "新乡市",
        code: "410700"
      },
      {
        name: "焦作市",
        code: "410800"
      },
      {
        name: "濮阳市",
        code: "410900"
      },
      {
        name: "许昌市",
        code: "411000"
      },
      {
        name: "漯河市",
        code: "411100"
      },
      {
        name: "三门峡市",
        code: "411200"
      },
      {
        name: "南阳市",
        code: "411300"
      },
      {
        name: "商丘市",
        code: "411400"
      },
      {
        name: "信阳市",
        code: "411500"
      },
      {
        name: "周口市",
        code: "411600"
      },
      {
        name: "驻马店市",
        code: "411700"
      },
      {
        name: "济源市",
        code: "419001"
      }
    ]
  },{
    name: "山西省",
    cities: [
      {
        name: "太原市",
        code: "140100"
      },
      {
        name: "大同市",
        code: "140200"
      },
      {
        name: "阳泉市",
        code: "140300"
      },
      {
        name: "长治市",
        code: "140400"
      },
      {
        name: "晋城市",
        code: "140500"
      },
      {
        name: "朔州市",
        code: "140600"
      },
      {
        name: "晋中市",
        code: "140700"
      },
      {
        name: "运城市",
        code: "140800"
      },
      {
        name: "忻州市",
        code: "140900"
      },
      {
        name: "临汾市",
        code: "141000"
      },
      {
        name: "吕梁市",
        code: "141100"
      }
    ]
  },{
    name: "陕西省",
    cities: [
      {
        name: "西安市",
        code: "610100"
      },
      {
        name: "铜川市",
        code: "610200"
      },
      {
        name: "宝鸡市",
        code: "610300"
      },
      {
        name: "咸阳市",
        code: "610400"
      },
      {
        name: "渭南市",
        code: "610500"
      },
      {
        name: "延安市",
        code: "610600"
      },
      {
        name: "汉中市",
        code: "610700"
      },
      {
        name: "榆林市",
        code: "610800"
      },
      {
        name: "安康市",
        code: "610900"
      },
      {
        name: "商洛市",
        code: "611000"
      }
    ]
  },{
    name: "湖北省",
    cities: [
      {
        name: "武汉市",
        code: "420100"
      },
      {
        name: "黄石市",
        code: "420200"
      },
      {
        name: "十堰市",
        code: "420300"
      },
      {
        name: "宜昌市",
        code: "420500"
      },
      {
        name: "襄阳市",
        code: "420600"
      },
      {
        name: "鄂州市",
        code: "420700"
      },
      {
        name: "荆门市",
        code: "420800"
      },
      {
        name: "孝感市",
        code: "420900"
      },
      {
        name: "荆州市",
        code: "421000"
      },
      {
        name: "黄冈市",
        code: "421100"
      },
      {
        name: "咸宁市",
        code: "421200"
      },
      {
        name: "随州市",
        code: "421300"
      },
      {
        name: "恩施土家族苗族自治州",
        code: "422800"
      },
      {
        name: "仙桃市",
        code: "429004"
      },
      {
        name: "潜江市",
        code: "429005"
      },
      {
        name: "天门市",
        code: "429006"
      },
      {
        name: "神农架林区",
        code: "429021"
      }
    ]
  },{
    name: "湖南省",
    cities: [
      {
        name: "长沙市",
        code: "430100"
      },
      {
        name: "株洲市",
        code: "430200"
      },
      {
        name: "湘潭市",
        code: "430300"
      },
      {
        name: "衡阳市",
        code: "430400"
      },
      {
        name: "邵阳市",
        code: "430500"
      },
      {
        name: "岳阳市",
        code: "430600"
      },
      {
        name: "常德市",
        code: "430700"
      },
      {
        name: "张家界市",
        code: "430800"
      },
      {
        name: "益阳市",
        code: "430900"
      },
      {
        name: "郴州市",
        code: "431000"
      },
      {
        name: "永州市",
        code: "431100"
      },
      {
        name: "怀化市",
        code: "431200"
      },
      {
        name: "娄底市",
        code: "431300"
      },
      {
        name: "湘西土家族苗族自治州",
        code: "433100"
      }
    ]
  },{
    name: "安徽省",
    cities: [
      {
        name: "合肥市",
        code: "340100"
      },
      {
        name: "芜湖市",
        code: "340200"
      },
      {
        name: "蚌埠市",
        code: "340300"
      },
      {
        name: "淮南市",
        code: "340400"
      },
      {
        name: "马鞍山市",
        code: "340500"
      },
      {
        name: "淮北市",
        code: "340600"
      },
      {
        name: "铜陵市",
        code: "340700"
      },
      {
        name: "安庆市",
        code: "340800"
      },
      {
        name: "黄山市",
        code: "341000"
      },
      {
        name: "滁州市",
        code: "341100"
      },
      {
        name: "阜阳市",
        code: "341200"
      },
      {
        name: "宿州市",
        code: "341300"
      },
      {
        name: "六安市",
        code: "341500"
      },
      {
        name: "亳州市",
        code: "341600"
      },
      {
        name: "池州市",
        code: "341700"
      },
      {
        name: "宣城市",
        code: "341800"
      }
    ]
  },{
    name: "江西省",
    cities: [
      {
        name: "南昌市",
        code: "360100"
      },
      {
        name: "景德镇市",
        code: "360200"
      },
      {
        name: "萍乡市",
        code: "360300"
      },
      {
        name: "九江市",
        code: "360400"
      },
      {
        name: "新余市",
        code: "360500"
      },
      {
        name: "鹰潭市",
        code: "360600"
      },
      {
        name: "赣州市",
        code: "360700"
      },
      {
        name: "吉安市",
        code: "360800"
      },
      {
        name: "宜春市",
        code: "360900"
      },
      {
        name: "抚州市",
        code: "361000"
      },
      {
        name: "上饶市",
        code: "361100"
      }
    ]
  }
  ],
  "美国": [
    {
      name: "加利福尼亚州",
      cities: [
        {
          name: "洛杉矶",
          code: "LA"
        }, {
          name: "旧金山",
          code: "SF"
        }
      ]
    }
  ]

};