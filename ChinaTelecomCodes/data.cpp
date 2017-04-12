#include "data.h"
#include <QString>
#include <QFile>
#include "database.h"
#include <QMessageBox>

int totalrecord;	//总记录数
ipdata ipcount[256*256];	//ip类型
urldata urlcount[1000];	//url统计
int getnum,postnum;	//页面提交方式
int re200,re302,re304,reothers;	//返回结果
int yearcount[100],monthcount[12],datecount[31],hourcount[4];	//时间
int earlyyear,no=0;	//最早的年份
int ua[45],os[5];	//浏览器和操作系统
int telcount[3];	//电话号码
int citycount133[345],citycount189[345];
int totalcitycount133[35],totalcitycount189[35];
int totalwidth=1024,totalheight=680;

int minSup,minConf;

QStringList province133=QStringList()<<QStringLiteral("北京")<<QStringLiteral("天津")<<QStringLiteral("河北")<<QStringLiteral("山西")<<QStringLiteral("内蒙")<<
	QStringLiteral("辽宁")<<QStringLiteral("吉林")<<QStringLiteral("黑龙江")<<QStringLiteral("上海")<<QStringLiteral("江苏")<<QStringLiteral("浙江")<<
	QStringLiteral("安徽")<<QStringLiteral("福建")<<QStringLiteral("江西")<<QStringLiteral("山东")<<QStringLiteral("河南")<<
	QStringLiteral("湖北")<<QStringLiteral("湖南")<<QStringLiteral("广东")<<QStringLiteral("广西")<<QStringLiteral("海南")<<
	QStringLiteral("四川")<<QStringLiteral("重庆")<<QStringLiteral("贵州")<<QStringLiteral("云南")<<QStringLiteral("陕西")<<
	QStringLiteral("甘肃")<<QStringLiteral("青海")<<QStringLiteral("宁夏")<<QStringLiteral("新疆")<<QStringLiteral("西藏");

int numofcity133[35]={0,1,2,13,24,36,50,59,72,73,86,97,114,123,134,151,169,183,197,218,231,234,255,256,265,281,291,305,314,319,335,343};

QStringList province189=QStringList()<<QStringLiteral("广东")<<QStringLiteral("江苏")<<QStringLiteral("浙江")<<QStringLiteral("四川")<<QStringLiteral("福建")<<
	QStringLiteral("湖北")<<QStringLiteral("上海")<<QStringLiteral("湖南")<<QStringLiteral("安徽")<<QStringLiteral("陕西")<<QStringLiteral("江西")<<
	QStringLiteral("广西")<<QStringLiteral("重庆")<<QStringLiteral("甘肃")<<QStringLiteral("山东")<<QStringLiteral("贵州")<<
	QStringLiteral("云南")<<QStringLiteral("新疆")<<QStringLiteral("辽宁")<<QStringLiteral("北京")<<QStringLiteral("河北")<<
	QStringLiteral("河南")<<QStringLiteral("海南")<<QStringLiteral("天津")<<QStringLiteral("黑龙江")<<QStringLiteral("吉林")<<
	QStringLiteral("山西")<<QStringLiteral("青海")<<QStringLiteral("宁夏")<<QStringLiteral("内蒙")<<QStringLiteral("西藏");

int numofcity189[35]={0,21,34,45,66,75,89,90,101,121,131,142,156,160,172,189,198,214,230,244,245,256,274,275,276,289,298,309,318,323,335,343};

QStringList cityname133=QStringList()<<QStringLiteral("北京")<<QStringLiteral("天津")<<QStringLiteral("石家庄")<<QStringLiteral("保定")<<QStringLiteral("张家口")<<QStringLiteral("承德")<<QStringLiteral("唐山")<<QStringLiteral("廊坊")<<QStringLiteral("沧州")<<QStringLiteral("衡水")<<
	QStringLiteral("邢台")<<QStringLiteral("邯郸")<<QStringLiteral("秦皇岛")<<QStringLiteral("太原")<<QStringLiteral("朔州")<<QStringLiteral("大同")<<QStringLiteral("阳泉")<<QStringLiteral("晋中")<<QStringLiteral("长治")<<QStringLiteral("晋城")<<
	QStringLiteral("临汾")<<QStringLiteral("吕梁")<<QStringLiteral("运城")<<QStringLiteral("忻州")<<QStringLiteral("呼和浩特")<<QStringLiteral("包头")<<QStringLiteral("乌海")<<QStringLiteral("乌兰查布")<<QStringLiteral("通辽")<<QStringLiteral("赤峰")<<
	QStringLiteral("鄂尔多斯")<<QStringLiteral("巴彦淖尔")<<QStringLiteral("锡林郭勒")<<QStringLiteral("呼伦贝尔")<<QStringLiteral("阿盟")<<QStringLiteral("兴安盟")<<QStringLiteral("沈阳")<<QStringLiteral("大连")<<QStringLiteral("鞍山")<<QStringLiteral("抚顺")<<
	QStringLiteral("本溪")<<QStringLiteral("丹东")<<QStringLiteral("铁岭")<<QStringLiteral("盘锦")<<QStringLiteral("锦州")<<QStringLiteral("营口")<<QStringLiteral("阜新")<<QStringLiteral("辽阳")<<QStringLiteral("朝阳")<<QStringLiteral("葫芦岛")<<
	QStringLiteral("长春")<<QStringLiteral("吉林")<<QStringLiteral("延边")<<QStringLiteral("四平")<<QStringLiteral("通化")<<QStringLiteral("白城")<<QStringLiteral("辽源")<<QStringLiteral("松原")<<QStringLiteral("白山")<<QStringLiteral("哈尔滨")<<
	QStringLiteral("齐齐哈尔")<<QStringLiteral("牡丹江")<<QStringLiteral("鸡西")<<QStringLiteral("七台河")<<QStringLiteral("佳木斯")<<QStringLiteral("鹤岗")<<QStringLiteral("双鸭山")<<QStringLiteral("绥化")<<QStringLiteral("黑河")<<QStringLiteral("大兴安岭")<<
	QStringLiteral("伊春")<<QStringLiteral("大庆")<<QStringLiteral("上海")<<QStringLiteral("南京")<<QStringLiteral("镇江")<<QStringLiteral("苏州")<<QStringLiteral("南通")<<QStringLiteral("扬州")<<QStringLiteral("盐城")<<QStringLiteral("徐州")<<
	QStringLiteral("淮安")<<QStringLiteral("连云港")<<QStringLiteral("常州")<<QStringLiteral("无锡")<<QStringLiteral("泰州")<<QStringLiteral("宿迁")<<QStringLiteral("杭州")<<QStringLiteral("湖州")<<QStringLiteral("嘉兴")<<QStringLiteral("宁波")<<
	QStringLiteral("绍兴")<<QStringLiteral("台州")<<QStringLiteral("温州")<<QStringLiteral("丽水")<<QStringLiteral("金华")<<QStringLiteral("衢州")<<QStringLiteral("舟山")<<QStringLiteral("合肥")<<QStringLiteral("蚌埠")<<QStringLiteral("芜湖")<<

	QStringLiteral("淮南")<<QStringLiteral("马鞍山")<<QStringLiteral("安庆")<<QStringLiteral("宿州")<<QStringLiteral("阜阳")<<QStringLiteral("亳州")<<QStringLiteral("黄山")<<QStringLiteral("滁州")<<QStringLiteral("淮北")<<QStringLiteral("铜陵")<<
	QStringLiteral("宣城")<<QStringLiteral("六安")<<QStringLiteral("巢湖")<<QStringLiteral("池州")<<QStringLiteral("福州")<<QStringLiteral("厦门")<<QStringLiteral("宁德")<<QStringLiteral("莆田")<<QStringLiteral("泉州")<<QStringLiteral("漳州")<<
	QStringLiteral("龙岩")<<QStringLiteral("三明")<<QStringLiteral("南平")<<QStringLiteral("南昌")<<QStringLiteral("九江")<<QStringLiteral("上饶")<<QStringLiteral("抚州")<<QStringLiteral("宜春")<<QStringLiteral("新余")<<QStringLiteral("萍乡")<<
	QStringLiteral("吉安")<<QStringLiteral("赣州")<<QStringLiteral("景德镇")<<QStringLiteral("鹰潭")<<QStringLiteral("济南")<<QStringLiteral("青岛")<<QStringLiteral("淄博")<<QStringLiteral("德州")<<QStringLiteral("烟台")<<QStringLiteral("潍坊")<<
	QStringLiteral("菏泽")<<QStringLiteral("济宁")<<QStringLiteral("泰安")<<QStringLiteral("临沂")<<QStringLiteral("滨州")<<QStringLiteral("东营")<<QStringLiteral("威海")<<QStringLiteral("枣庄")<<QStringLiteral("日照")<<QStringLiteral("莱芜")<<
	QStringLiteral("聊城")<<QStringLiteral("郑州")<<QStringLiteral("安阳")<<QStringLiteral("新乡")<<QStringLiteral("许昌")<<QStringLiteral("平顶山")<<QStringLiteral("信阳")<<QStringLiteral("南阳")<<QStringLiteral("开封")<<QStringLiteral("洛阳")<<
	QStringLiteral("商丘")<<QStringLiteral("焦作")<<QStringLiteral("鹤壁")<<QStringLiteral("濮阳")<<QStringLiteral("周口")<<QStringLiteral("漯河")<<QStringLiteral("驻马店")<<QStringLiteral("潢川")<<QStringLiteral("三门峡")<<QStringLiteral("武汉")<<
	QStringLiteral("鄂州")<<QStringLiteral("孝感")<<QStringLiteral("黄冈")<<QStringLiteral("黄石")<<QStringLiteral("咸宁")<<QStringLiteral("荆州")<<QStringLiteral("宜昌")<<QStringLiteral("恩施")<<QStringLiteral("十堰")<<QStringLiteral("襄樊")<<
	QStringLiteral("随州")<<QStringLiteral("荆门")<<QStringLiteral("仙桃")<<QStringLiteral("长沙")<<QStringLiteral("湘潭")<<QStringLiteral("株洲")<<QStringLiteral("衡阳")<<QStringLiteral("郴州")<<QStringLiteral("常德")<<QStringLiteral("益阳")<<
	QStringLiteral("娄底")<<QStringLiteral("邵阳")<<QStringLiteral("岳阳")<<QStringLiteral("吉首")<<QStringLiteral("张家界")<<QStringLiteral("怀化")<<QStringLiteral("永州")<<QStringLiteral("广州")<<QStringLiteral("江门")<<QStringLiteral("韶关")<<

	QStringLiteral("惠州")<<QStringLiteral("梅州")<<QStringLiteral("汕头")<<QStringLiteral("深圳")<<QStringLiteral("珠海")<<QStringLiteral("佛山")<<QStringLiteral("肇庆")<<QStringLiteral("湛江")<<QStringLiteral("中山")<<QStringLiteral("河源")<<
	QStringLiteral("清远")<<QStringLiteral("云浮")<<QStringLiteral("潮州")<<QStringLiteral("东莞")<<QStringLiteral("汕尾")<<QStringLiteral("阳江")<<QStringLiteral("揭阳")<<QStringLiteral("茂名")<<QStringLiteral("南宁")<<QStringLiteral("柳州")<<	
	QStringLiteral("来宾")<<QStringLiteral("桂林")<<QStringLiteral("贺州")<<QStringLiteral("梧州")<<QStringLiteral("玉林")<<QStringLiteral("贵港")<<QStringLiteral("百色")<<QStringLiteral("钦州")<<QStringLiteral("河池")<<QStringLiteral("北海")<<
	QStringLiteral("防城港")<<QStringLiteral("海口")<<QStringLiteral("三亚")<<QStringLiteral("儋州")<<QStringLiteral("成都")<<QStringLiteral("攀枝花")<<QStringLiteral("自贡")<<QStringLiteral("绵阳")<<QStringLiteral("南充")<<QStringLiteral("达州")<<
	QStringLiteral("遂宁")<<QStringLiteral("广安")<<QStringLiteral("巴中")<<QStringLiteral("宜宾")<<QStringLiteral("内江")<<QStringLiteral("资阳")<<QStringLiteral("乐山")<<QStringLiteral("眉山")<<QStringLiteral("西昌")<<QStringLiteral("雅安")<<
	QStringLiteral("甘孜")<<QStringLiteral("阿坝")<<QStringLiteral("德阳")<<QStringLiteral("广元")<<QStringLiteral("泸州")<<QStringLiteral("重庆")<<QStringLiteral("贵阳")<<QStringLiteral("遵义")<<QStringLiteral("安顺")<<QStringLiteral("都匀")<<
	QStringLiteral("凯里")<<QStringLiteral("铜仁")<<QStringLiteral("毕节")<<QStringLiteral("六盘水")<<QStringLiteral("兴义")<<QStringLiteral("昆明")<<QStringLiteral("大理")<<QStringLiteral("红河")<<QStringLiteral("曲靖")<<QStringLiteral("玉溪")<<
	QStringLiteral("楚雄")<<QStringLiteral("昭通")<<QStringLiteral("临沧")<<QStringLiteral("怒江")<<QStringLiteral("迪庆")<<QStringLiteral("丽江")<<QStringLiteral("版纳")<<QStringLiteral("德宏")<<QStringLiteral("文山")<<QStringLiteral("思茅")<<
	QStringLiteral("宝山")<<QStringLiteral("西安")<<QStringLiteral("延安")<<QStringLiteral("榆林")<<QStringLiteral("渭南")<<QStringLiteral("商洛")<<QStringLiteral("安康")<<QStringLiteral("汉中")<<QStringLiteral("宝鸡")<<QStringLiteral("铜川")<<
	QStringLiteral("咸阳")<<QStringLiteral("兰州")<<QStringLiteral("定西")<<QStringLiteral("平凉")<<QStringLiteral("庆阳")<<QStringLiteral("武威")<<QStringLiteral("金昌")<<QStringLiteral("张掖")<<QStringLiteral("嘉峪关")<<QStringLiteral("酒泉")<<

	QStringLiteral("天水")<<QStringLiteral("陇南")<<QStringLiteral("临夏")<<QStringLiteral("甘南")<<QStringLiteral("白银")<<QStringLiteral("西宁")<<QStringLiteral("海东地区")<<QStringLiteral("黄南州")<<QStringLiteral("海南州")<<QStringLiteral("果洛州")<<
	QStringLiteral("玉树州")<<QStringLiteral("海西州")<<QStringLiteral("格尔木")<<QStringLiteral("海北州")<<QStringLiteral("银川")<<QStringLiteral("石嘴山")<<QStringLiteral("吴忠")<<QStringLiteral("固原")<<QStringLiteral("中卫")<<QStringLiteral("乌鲁木齐")<<
	QStringLiteral("奎屯")<<QStringLiteral("石河子")<<QStringLiteral("昌吉")<<QStringLiteral("吐鲁番")<<QStringLiteral("库尔勒")<<QStringLiteral("阿克苏")<<QStringLiteral("喀什")<<QStringLiteral("伊犁")<<QStringLiteral("克拉玛依")<<QStringLiteral("哈密")<<
	QStringLiteral("克州")<<QStringLiteral("博乐")<<QStringLiteral("阿勒泰")<<QStringLiteral("和田")<<QStringLiteral("塔城")<<QStringLiteral("拉萨")<<QStringLiteral("日喀则")<<QStringLiteral("山南")<<QStringLiteral("林芝")<<QStringLiteral("昌都")<<
	QStringLiteral("那曲")<<QStringLiteral("阿里");

QStringList cityname189=QStringList()<<QStringLiteral("广州")<<QStringLiteral("深圳")<<QStringLiteral("东莞")<<QStringLiteral("佛山")<<QStringLiteral("汕尾")<<QStringLiteral("阳江")<<QStringLiteral("揭阳")<<QStringLiteral("茂名")<<QStringLiteral("江门")<<QStringLiteral("韶关")<<
	QStringLiteral("惠州")<<QStringLiteral("梅州")<<QStringLiteral("汕头")<<QStringLiteral("珠海")<<QStringLiteral("肇庆")<<QStringLiteral("湛江")<<QStringLiteral("中山")<<QStringLiteral("河源")<<QStringLiteral("清远")<<QStringLiteral("云浮")<<
	QStringLiteral("潮州")<<QStringLiteral("南京")<<QStringLiteral("无锡")<<QStringLiteral("镇江")<<QStringLiteral("苏州")<<QStringLiteral("南通")<<QStringLiteral("扬州")<<QStringLiteral("盐城")<<QStringLiteral("徐州")<<QStringLiteral("淮安")<<
	QStringLiteral("连云港")<<QStringLiteral("常州")<<QStringLiteral("泰州")<<QStringLiteral("宿迁")<<QStringLiteral("杭州")<<QStringLiteral("宁波")<<QStringLiteral("温州")<<QStringLiteral("嘉兴")<<QStringLiteral("湖州")<<QStringLiteral("绍兴")<<
	QStringLiteral("金华")<<QStringLiteral("衢州")<<QStringLiteral("丽水")<<QStringLiteral("台州")<<QStringLiteral("舟山")<<QStringLiteral("成都")<<QStringLiteral("广元")<<QStringLiteral("绵阳")<<QStringLiteral("乐山")<<QStringLiteral("广安")<<
	QStringLiteral("巴中")<<QStringLiteral("宜宾")<<QStringLiteral("阿坝")<<QStringLiteral("甘孜")<<QStringLiteral("内江")<<QStringLiteral("南充")<<QStringLiteral("凉山")<<QStringLiteral("泸州")<<QStringLiteral("遂宁")<<QStringLiteral("雅安")<<
	QStringLiteral("自贡")<<QStringLiteral("资阳")<<QStringLiteral("攀枝花")<<QStringLiteral("达州")<<QStringLiteral("德阳")<<QStringLiteral("眉山")<<QStringLiteral("福州")<<QStringLiteral("厦门")<<QStringLiteral("宁德")<<QStringLiteral("莆田")<<
	QStringLiteral("泉州")<<QStringLiteral("漳州")<<QStringLiteral("龙岩")<<QStringLiteral("三明")<<QStringLiteral("南平")<<QStringLiteral("武汉")<<QStringLiteral("襄樊")<<QStringLiteral("鄂州")<<QStringLiteral("孝感")<<QStringLiteral("黄冈")<<
	QStringLiteral("黄石")<<QStringLiteral("咸宁")<<QStringLiteral("荆州")<<QStringLiteral("宜昌")<<QStringLiteral("恩施")<<QStringLiteral("十堰")<<QStringLiteral("随州")<<QStringLiteral("荆门")<<QStringLiteral("江汉")<<QStringLiteral("上海")<<
	QStringLiteral("岳阳")<<QStringLiteral("长沙")<<QStringLiteral("湘潭")<<QStringLiteral("株洲")<<QStringLiteral("衡阳")<<QStringLiteral("郴州")<<QStringLiteral("常德")<<QStringLiteral("益阳")<<QStringLiteral("娄底")<<QStringLiteral("邵阳")<<

	QStringLiteral("自治州")<<QStringLiteral("张家界")<<QStringLiteral("怀化")<<QStringLiteral("永州")<<QStringLiteral("合肥")<<QStringLiteral("蚌埠")<<QStringLiteral("芜湖")<<QStringLiteral("淮南")<<QStringLiteral("马鞍山")<<QStringLiteral("安庆")<<
	QStringLiteral("宿州")<<QStringLiteral("阜阳")<<QStringLiteral("亳州")<<QStringLiteral("黄山")<<QStringLiteral("滁州")<<QStringLiteral("淮北")<<QStringLiteral("铜陵")<<QStringLiteral("宣城")<<QStringLiteral("六安")<<QStringLiteral("巢湖")<<
	QStringLiteral("池州")<<QStringLiteral("西安")<<QStringLiteral("咸阳")<<QStringLiteral("延安")<<QStringLiteral("榆林")<<QStringLiteral("渭南")<<QStringLiteral("商洛")<<QStringLiteral("安康")<<QStringLiteral("汉中")<<QStringLiteral("宝鸡")<<
	QStringLiteral("铜川")<<QStringLiteral("南昌")<<QStringLiteral("九江")<<QStringLiteral("上饶")<<QStringLiteral("抚州")<<QStringLiteral("宜春")<<QStringLiteral("吉安")<<QStringLiteral("赣州")<<QStringLiteral("景德镇")<<QStringLiteral("萍乡")<<
	QStringLiteral("新余")<<QStringLiteral("鹰潭")<<QStringLiteral("南宁")<<QStringLiteral("崇左")<<QStringLiteral("柳州")<<QStringLiteral("来宾")<<QStringLiteral("桂林")<<QStringLiteral("梧州")<<QStringLiteral("贺州")<<QStringLiteral("玉林")<<
	QStringLiteral("贵港")<<QStringLiteral("百色")<<QStringLiteral("钦州")<<QStringLiteral("河池")<<QStringLiteral("北海")<<QStringLiteral("防城港")<<QStringLiteral("重庆")<<QStringLiteral("万州")<<QStringLiteral("涪陵")<<QStringLiteral("黔江")<<
	QStringLiteral("临夏")<<QStringLiteral("兰州")<<QStringLiteral("天水")<<QStringLiteral("定西")<<QStringLiteral("平凉")<<QStringLiteral("庆阳")<<QStringLiteral("张掖")<<QStringLiteral("甘南")<<QStringLiteral("白银")<<QStringLiteral("酒泉/嘉峪关")<<
	QStringLiteral("金昌/武威")<<QStringLiteral("陇南")<<QStringLiteral("菏泽")<<QStringLiteral("济南")<<QStringLiteral("青岛")<<QStringLiteral("淄博")<<QStringLiteral("德州")<<QStringLiteral("烟台")<<QStringLiteral("潍坊")<<QStringLiteral("济宁")<<
	QStringLiteral("泰安")<<QStringLiteral("临沂")<<QStringLiteral("滨州")<<QStringLiteral("东营")<<QStringLiteral("威海")<<QStringLiteral("枣庄")<<QStringLiteral("日照")<<QStringLiteral("莱芜")<<QStringLiteral("聊城")<<QStringLiteral("贵阳")<<
	QStringLiteral("遵义")<<QStringLiteral("安顺")<<QStringLiteral("黔南")<<QStringLiteral("黔东南")<<QStringLiteral("铜仁")<<QStringLiteral("毕节")<<QStringLiteral("六盘水")<<QStringLiteral("黔西南")<<QStringLiteral("版纳")<<QStringLiteral("德宏")<<

	QStringLiteral("昭通")<<QStringLiteral("昆明")<<QStringLiteral("大理")<<QStringLiteral("红河")<<QStringLiteral("曲靖")<<QStringLiteral("保山")<<QStringLiteral("文山")<<QStringLiteral("玉溪")<<QStringLiteral("楚雄")<<QStringLiteral("普洱")<<
	QStringLiteral("临沧")<<QStringLiteral("怒江")<<QStringLiteral("迪庆")<<QStringLiteral("丽江")<<QStringLiteral("乌鲁木齐")<<QStringLiteral("昌吉")<<QStringLiteral("巴州")<<QStringLiteral("伊犁")<<QStringLiteral("阿克苏")<<QStringLiteral("喀什")<<
	QStringLiteral("塔城")<<QStringLiteral("石河子")<<QStringLiteral("阿勒泰")<<QStringLiteral("和田")<<QStringLiteral("克拉玛依")<<QStringLiteral("哈密")<<QStringLiteral("吐鲁番")<<QStringLiteral("博乐")<<QStringLiteral("奎屯")<<QStringLiteral("克州")<<
	QStringLiteral("沈阳")<<QStringLiteral("大连")<<QStringLiteral("鞍山")<<QStringLiteral("抚顺")<<QStringLiteral("本溪")<<QStringLiteral("丹东")<<QStringLiteral("锦州")<<QStringLiteral("营口")<<QStringLiteral("阜新")<<QStringLiteral("辽阳")<<
	QStringLiteral("铁岭")<<QStringLiteral("朝阳")<<QStringLiteral("盘锦")<<QStringLiteral("葫芦岛")<<QStringLiteral("北京")<<QStringLiteral("石家庄")<<QStringLiteral("唐山")<<QStringLiteral("秦皇岛")<<QStringLiteral("邯郸")<<QStringLiteral("邢台")<<
	QStringLiteral("保定")<<QStringLiteral("廊坊")<<QStringLiteral("张家口")<<QStringLiteral("沧州")<<QStringLiteral("衡水")<<QStringLiteral("承德")<<QStringLiteral("三门峡")<<QStringLiteral("信阳")<<QStringLiteral("南阳")<<QStringLiteral("周口")<<
	QStringLiteral("商丘")<<QStringLiteral("安阳")<<QStringLiteral("平顶山")<<QStringLiteral("开封")<<QStringLiteral("新乡")<<QStringLiteral("洛阳")<<QStringLiteral("漯河")<<QStringLiteral("潢川")<<QStringLiteral("濮阳")<<QStringLiteral("焦作")<<
	QStringLiteral("许昌")<<QStringLiteral("郑州")<<QStringLiteral("驻马店")<<QStringLiteral("鹤壁")<<QStringLiteral("海南")<<QStringLiteral("天津")<<QStringLiteral("哈尔滨")<<QStringLiteral("齐齐哈尔")<<QStringLiteral("牡丹江")<<QStringLiteral("佳木斯")<<
	QStringLiteral("绥化")<<QStringLiteral("黑河")<<QStringLiteral("大兴安岭")<<QStringLiteral("伊春")<<QStringLiteral("大庆")<<QStringLiteral("七台河")<<QStringLiteral("鸡西")<<QStringLiteral("鹤岗")<<QStringLiteral("双鸭山")<<QStringLiteral("长春")<<
	QStringLiteral("吉林")<<QStringLiteral("延边")<<QStringLiteral("四平")<<QStringLiteral("通化")<<QStringLiteral("白城")<<QStringLiteral("辽源")<<QStringLiteral("松原")<<QStringLiteral("白山")<<QStringLiteral("朔州")<<QStringLiteral("忻州")<<

	QStringLiteral("太原")<<QStringLiteral("大同")<<QStringLiteral("阳泉")<<QStringLiteral("晋中")<<QStringLiteral("长治")<<QStringLiteral("晋城")<<QStringLiteral("临汾")<<QStringLiteral("吕梁")<<QStringLiteral("运城")<<QStringLiteral("西宁")<<
	QStringLiteral("海东")<<QStringLiteral("海西")<<QStringLiteral("海南")<<QStringLiteral("海北")<<QStringLiteral("黄南")<<QStringLiteral("果洛")<<QStringLiteral("玉树")<<QStringLiteral("格尔木")<<QStringLiteral("银川")<<QStringLiteral("石嘴山")<<
	QStringLiteral("吴忠")<<QStringLiteral("固原")<<QStringLiteral("中卫")<<QStringLiteral("呼伦贝尔")<<QStringLiteral("呼和浩特")<<QStringLiteral("包头")<<QStringLiteral("乌海")<<QStringLiteral("乌兰察布")<<QStringLiteral("通辽")<<QStringLiteral("赤峰")<<
	QStringLiteral("鄂尔多斯")<<QStringLiteral("巴彦淖尔")<<QStringLiteral("锡盟")<<QStringLiteral("兴安盟")<<QStringLiteral("阿盟")<<QStringLiteral("拉萨")<<QStringLiteral("日喀则")<<QStringLiteral("山南")<<QStringLiteral("林芝")<<QStringLiteral("昌都")<<
	QStringLiteral("那曲")<<QStringLiteral("阿里");

QStringList uaname=QStringList()<<QStringLiteral("三星")<<QStringLiteral("摩托罗拉")<<QStringLiteral("宇龙")<<"LG"<<QStringLiteral("中兴")<<QStringLiteral("联想")<<QStringLiteral("海信")<<QStringLiteral("大唐")<<QStringLiteral("广信")<<QStringLiteral("富春江")
	<<QStringLiteral("德信")<<QStringLiteral("东森")<<QStringLiteral("海尔")<<QStringLiteral("创维")<<QStringLiteral("华录")<<QStringLiteral("飞利浦")<<QStringLiteral("黑莓")<<QStringLiteral("诺基亚")<<"HTC"<<"TCL"
	<<QStringLiteral("UT斯达康")<<QStringLiteral("大显")<<QStringLiteral("恒基伟业")<<QStringLiteral("华为")<<QStringLiteral("金淼")<<QStringLiteral("盟宝")<<QStringLiteral("世纪天元")<<QStringLiteral("天宇")<<QStringLiteral("易丰")<<"OKWAP"
	<<QStringLiteral("华立")<<QStringLiteral("新中桥")<<QStringLiteral("雷梦")<<QStringLiteral("金立")<<"NEC"<<"CECT"<<QStringLiteral("宏康")<<QStringLiteral("盛泰")<<QStringLiteral("波导")<<QStringLiteral("多普达")
	<<QStringLiteral("三普")<<QStringLiteral("夏新");

QStringList uachar=QStringList()<<"SCH"<<"MOT"<<"YL-COOLPAD"<<"LG"<<"ZTE"<<"LNV-Lenovo"<<"HX-"<<"DT-"<<"GX"<<"FCJ-"
	<<"QIGI"<<"DS-"<<"HC-"<<"CW-"<<"CHL-"<<"SAF"<<"BlackBerry"<<"NOK-"<<"HTC-"<<"TCL-"
	<<"UT-"<<"DX-"<<"HTW-"<<"HW-"<<"NAIDE-"<<"MOB-"<<"YCT-"<<"TY-"<<"YFZ"<<"OKW-"
	<<"HL-"<<"CB-"<<"TG-"<<"GIONEE"<<"NEC-"<<"CECT-"<<"HK-"<<"CKING-"<<"BD-"<<"DOPOD-"
	<<"MMX-"<<"AMOI-";

QStringList osname=QStringList()<<"android"<<"Linux"<<"windows"<<"bada";

int citynum(const char *tel) {
	int midthree;
	midthree=(tel[4]-'0')*100+(tel[5]-'0')*10+tel[6]-'0';
	//判断数据表是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {
		QMessageBox message(QMessageBox::NoIcon,"error!","Please read the city files first!");
		message.setIconPixmap(QPixmap("./image/icon.png"));
		message.exec();
		return -1;
	}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")||!tables.contains("telToCity133")||!tables.contains("telToCity153")||!tables.contains("telToCity189")) {
		QMessageBox message(QMessageBox::NoIcon,"wrong data:","no table telToCity133 or telToCity153 or telToCity189!\nPlease read the city files first!");
		message.setIconPixmap(QPixmap("./image/icon.png"));
		message.exec();
		return -1;
	}
	QSqlQuery query;
	switch(tel[1]) {
	case '3':
		query.exec(QString("select * from telToCity133 where midthree=%1").arg(midthree));
		query.next();
		return query.value(tel[4]-'0').toInt();
	case '5':
		query.exec(QString("select * from telToCity153 where midthree=%1").arg(midthree));
		query.next();
		return query.value(tel[4]-'0').toInt();
	case '8':
		query.exec(QString("select * from telToCity153 where midthree=%1").arg(midthree));
		query.next();
		return query.value(tel[4]-'0').toInt();
	default:return -1;
	}
}

//Use for terminal capture
vector<dataTime> datatime;
vector<list<dataCompany> > datacompany;
vector<list<dataCompany> > newtml;

//UV-PV graphics
dataTime begintime, endtime;

void timespan()
{
	QFile *databasefile = new QFile;
	if (!databasefile->exists("./database.db")) { QMessageBox::warning(NULL, "error!", "Please read the log files first!"); return; }
	Database db;
	db.open();
	QStringList tables;
	tables = db.db.tables();
	if (!tables.contains("totalTable")) {
		QMessageBox::warning(NULL, "Warning", "There is no database.db file, please read log file first!");
		return;
	}
	QSqlQuery query;
	query.exec("select years,months,dates,count(*) from totalTable group by years,months,dates");
	datatime.clear();
	dataTime tmpDatatime;
	while (query.next())	{
		tmpDatatime.year = query.value(0).toInt();
		tmpDatatime.month = query.value(1).toInt();
		tmpDatatime.date = query.value(2).toInt();
		tmpDatatime.num = query.value(3).toInt();
		datatime.push_back(tmpDatatime);
	}

	db.close();

	begintime = datatime[0];
	endtime = datatime[datatime.size() - 1];
}