#include "data.h"
#include <QString>
#include <QFile>
#include "database.h"
#include <QMessageBox>

int totalrecord;	//�ܼ�¼��
ipdata ipcount[256*256];	//ip����
urldata urlcount[1000];	//urlͳ��
int getnum,postnum;	//ҳ���ύ��ʽ
int re200,re302,re304,reothers;	//���ؽ��
int yearcount[100],monthcount[12],datecount[31],hourcount[4];	//ʱ��
int earlyyear,no=0;	//��������
int ua[45],os[5];	//������Ͳ���ϵͳ
int telcount[3];	//�绰����
int citycount133[345],citycount189[345];
int totalcitycount133[35],totalcitycount189[35];
int totalwidth=1024,totalheight=680;

int minSup,minConf;

QStringList province133=QStringList()<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("�ӱ�")<<QStringLiteral("ɽ��")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("������")<<QStringLiteral("�Ϻ�")<<QStringLiteral("����")<<QStringLiteral("�㽭")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ɽ��")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�㶫")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("�Ĵ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("�ຣ")<<QStringLiteral("����")<<QStringLiteral("�½�")<<QStringLiteral("����");

int numofcity133[35]={0,1,2,13,24,36,50,59,72,73,86,97,114,123,134,151,169,183,197,218,231,234,255,256,265,281,291,305,314,319,335,343};

QStringList province189=QStringList()<<QStringLiteral("�㶫")<<QStringLiteral("����")<<QStringLiteral("�㽭")<<QStringLiteral("�Ĵ�")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("�Ϻ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ɽ��")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("�½�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ӱ�")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("������")<<QStringLiteral("����")<<
	QStringLiteral("ɽ��")<<QStringLiteral("�ຣ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����");

int numofcity189[35]={0,21,34,45,66,75,89,90,101,121,131,142,156,160,172,189,198,214,230,244,245,256,274,275,276,289,298,309,318,323,335,343};

QStringList cityname133=QStringList()<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("ʯ��ׯ")<<QStringLiteral("����")<<QStringLiteral("�żҿ�")<<QStringLiteral("�е�")<<QStringLiteral("��ɽ")<<QStringLiteral("�ȷ�")<<QStringLiteral("����")<<QStringLiteral("��ˮ")<<
	QStringLiteral("��̨")<<QStringLiteral("����")<<QStringLiteral("�ػʵ�")<<QStringLiteral("̫ԭ")<<QStringLiteral("˷��")<<QStringLiteral("��ͬ")<<QStringLiteral("��Ȫ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("�ٷ�")<<QStringLiteral("����")<<QStringLiteral("�˳�")<<QStringLiteral("����")<<QStringLiteral("���ͺ���")<<QStringLiteral("��ͷ")<<QStringLiteral("�ں�")<<QStringLiteral("�����鲼")<<QStringLiteral("ͨ��")<<QStringLiteral("���")<<
	QStringLiteral("������˹")<<QStringLiteral("�����׶�")<<QStringLiteral("���ֹ���")<<QStringLiteral("���ױ���")<<QStringLiteral("����")<<QStringLiteral("�˰���")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("��˳")<<
	QStringLiteral("��Ϫ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�̽�")<<QStringLiteral("����")<<QStringLiteral("Ӫ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��«��")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ӱ�")<<QStringLiteral("��ƽ")<<QStringLiteral("ͨ��")<<QStringLiteral("�׳�")<<QStringLiteral("��Դ")<<QStringLiteral("��ԭ")<<QStringLiteral("��ɽ")<<QStringLiteral("������")<<
	QStringLiteral("�������")<<QStringLiteral("ĵ����")<<QStringLiteral("����")<<QStringLiteral("��̨��")<<QStringLiteral("��ľ˹")<<QStringLiteral("�׸�")<<QStringLiteral("˫Ѽɽ")<<QStringLiteral("�绯")<<QStringLiteral("�ں�")<<QStringLiteral("���˰���")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�Ϻ�")<<QStringLiteral("�Ͼ�")<<QStringLiteral("��")<<QStringLiteral("����")<<QStringLiteral("��ͨ")<<QStringLiteral("����")<<QStringLiteral("�γ�")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("���Ƹ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("̩��")<<QStringLiteral("��Ǩ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("̨��")<<QStringLiteral("����")<<QStringLiteral("��ˮ")<<QStringLiteral("��")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("�Ϸ�")<<QStringLiteral("����")<<QStringLiteral("�ߺ�")<<

	QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ͭ��")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("Ȫ��")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ƽ")<<QStringLiteral("�ϲ�")<<QStringLiteral("�Ž�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�˴�")<<QStringLiteral("����")<<QStringLiteral("Ƽ��")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("������")<<QStringLiteral("ӥ̶")<<QStringLiteral("����")<<QStringLiteral("�ൺ")<<QStringLiteral("�Ͳ�")<<QStringLiteral("����")<<QStringLiteral("��̨")<<QStringLiteral("Ϋ��")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("̩��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��Ӫ")<<QStringLiteral("����")<<QStringLiteral("��ׯ")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("�ĳ�")<<QStringLiteral("֣��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("ƽ��ɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ױ�")<<QStringLiteral("���")<<QStringLiteral("�ܿ�")<<QStringLiteral("���")<<QStringLiteral("פ���")<<QStringLiteral("�괨")<<QStringLiteral("����Ͽ")<<QStringLiteral("�人")<<
	QStringLiteral("����")<<QStringLiteral("Т��")<<QStringLiteral("�Ƹ�")<<QStringLiteral("��ʯ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�˲�")<<QStringLiteral("��ʩ")<<QStringLiteral("ʮ��")<<QStringLiteral("�差")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ɳ")<<QStringLiteral("��̶")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("¦��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�żҽ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ع�")<<

	QStringLiteral("����")<<QStringLiteral("÷��")<<QStringLiteral("��ͷ")<<QStringLiteral("����")<<QStringLiteral("�麣")<<QStringLiteral("��ɽ")<<QStringLiteral("����")<<QStringLiteral("տ��")<<QStringLiteral("��ɽ")<<QStringLiteral("��Դ")<<
	QStringLiteral("��Զ")<<QStringLiteral("�Ƹ�")<<QStringLiteral("����")<<QStringLiteral("��ݸ")<<QStringLiteral("��β")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ï��")<<QStringLiteral("����")<<QStringLiteral("����")<<	
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("��ɫ")<<QStringLiteral("����")<<QStringLiteral("�ӳ�")<<QStringLiteral("����")<<
	QStringLiteral("���Ǹ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ɶ�")<<QStringLiteral("��֦��")<<QStringLiteral("�Թ�")<<QStringLiteral("����")<<QStringLiteral("�ϳ�")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("�㰲")<<QStringLiteral("����")<<QStringLiteral("�˱�")<<QStringLiteral("�ڽ�")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("üɽ")<<QStringLiteral("����")<<QStringLiteral("�Ű�")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��Ԫ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��˳")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("ͭ��")<<QStringLiteral("�Ͻ�")<<QStringLiteral("����ˮ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("����")<<QStringLiteral("��Ϫ")<<
	QStringLiteral("����")<<QStringLiteral("��ͨ")<<QStringLiteral("�ٲ�")<<QStringLiteral("ŭ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�º�")<<QStringLiteral("��ɽ")<<QStringLiteral("˼é")<<
	QStringLiteral("��ɽ")<<QStringLiteral("����")<<QStringLiteral("�Ӱ�")<<QStringLiteral("����")<<QStringLiteral("μ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ͭ��")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ƽ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("��Ҵ")<<QStringLiteral("������")<<QStringLiteral("��Ȫ")<<

	QStringLiteral("��ˮ")<<QStringLiteral("¤��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��������")<<QStringLiteral("������")<<QStringLiteral("������")<<QStringLiteral("������")<<
	QStringLiteral("������")<<QStringLiteral("������")<<QStringLiteral("���ľ")<<QStringLiteral("������")<<QStringLiteral("����")<<QStringLiteral("ʯ��ɽ")<<QStringLiteral("����")<<QStringLiteral("��ԭ")<<QStringLiteral("����")<<QStringLiteral("��³ľ��")<<
	QStringLiteral("����")<<QStringLiteral("ʯ����")<<QStringLiteral("����")<<QStringLiteral("��³��")<<QStringLiteral("�����")<<QStringLiteral("������")<<QStringLiteral("��ʲ")<<QStringLiteral("����")<<QStringLiteral("��������")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����̩")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�տ���")<<QStringLiteral("ɽ��")<<QStringLiteral("��֥")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����");

QStringList cityname189=QStringList()<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ݸ")<<QStringLiteral("��ɽ")<<QStringLiteral("��β")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ï��")<<QStringLiteral("����")<<QStringLiteral("�ع�")<<
	QStringLiteral("����")<<QStringLiteral("÷��")<<QStringLiteral("��ͷ")<<QStringLiteral("�麣")<<QStringLiteral("����")<<QStringLiteral("տ��")<<QStringLiteral("��ɽ")<<QStringLiteral("��Դ")<<QStringLiteral("��Զ")<<QStringLiteral("�Ƹ�")<<
	QStringLiteral("����")<<QStringLiteral("�Ͼ�")<<QStringLiteral("����")<<QStringLiteral("��")<<QStringLiteral("����")<<QStringLiteral("��ͨ")<<QStringLiteral("����")<<QStringLiteral("�γ�")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("���Ƹ�")<<QStringLiteral("����")<<QStringLiteral("̩��")<<QStringLiteral("��Ǩ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("��")<<QStringLiteral("����")<<QStringLiteral("��ˮ")<<QStringLiteral("̨��")<<QStringLiteral("��ɽ")<<QStringLiteral("�ɶ�")<<QStringLiteral("��Ԫ")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("�㰲")<<
	QStringLiteral("����")<<QStringLiteral("�˱�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ڽ�")<<QStringLiteral("�ϳ�")<<QStringLiteral("��ɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�Ű�")<<
	QStringLiteral("�Թ�")<<QStringLiteral("����")<<QStringLiteral("��֦��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("üɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("Ȫ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ƽ")<<QStringLiteral("�人")<<QStringLiteral("�差")<<QStringLiteral("����")<<QStringLiteral("Т��")<<QStringLiteral("�Ƹ�")<<
	QStringLiteral("��ʯ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�˲�")<<QStringLiteral("��ʩ")<<QStringLiteral("ʮ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�Ϻ�")<<
	QStringLiteral("����")<<QStringLiteral("��ɳ")<<QStringLiteral("��̶")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("¦��")<<QStringLiteral("����")<<

	QStringLiteral("������")<<QStringLiteral("�żҽ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�Ϸ�")<<QStringLiteral("����")<<QStringLiteral("�ߺ�")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ͭ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�Ӱ�")<<QStringLiteral("����")<<QStringLiteral("μ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("ͭ��")<<QStringLiteral("�ϲ�")<<QStringLiteral("�Ž�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�˴�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("������")<<QStringLiteral("Ƽ��")<<
	QStringLiteral("����")<<QStringLiteral("ӥ̶")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("���")<<QStringLiteral("��ɫ")<<QStringLiteral("����")<<QStringLiteral("�ӳ�")<<QStringLiteral("����")<<QStringLiteral("���Ǹ�")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ǭ��")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ˮ")<<QStringLiteral("����")<<QStringLiteral("ƽ��")<<QStringLiteral("����")<<QStringLiteral("��Ҵ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��Ȫ/������")<<
	QStringLiteral("���/����")<<QStringLiteral("¤��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ൺ")<<QStringLiteral("�Ͳ�")<<QStringLiteral("����")<<QStringLiteral("��̨")<<QStringLiteral("Ϋ��")<<QStringLiteral("����")<<
	QStringLiteral("̩��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��Ӫ")<<QStringLiteral("����")<<QStringLiteral("��ׯ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ĳ�")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("��˳")<<QStringLiteral("ǭ��")<<QStringLiteral("ǭ����")<<QStringLiteral("ͭ��")<<QStringLiteral("�Ͻ�")<<QStringLiteral("����ˮ")<<QStringLiteral("ǭ����")<<QStringLiteral("����")<<QStringLiteral("�º�")<<

	QStringLiteral("��ͨ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("��ɽ")<<QStringLiteral("��Ϫ")<<QStringLiteral("����")<<QStringLiteral("�ն�")<<
	QStringLiteral("�ٲ�")<<QStringLiteral("ŭ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��³ľ��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("������")<<QStringLiteral("��ʲ")<<
	QStringLiteral("����")<<QStringLiteral("ʯ����")<<QStringLiteral("����̩")<<QStringLiteral("����")<<QStringLiteral("��������")<<QStringLiteral("����")<<QStringLiteral("��³��")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��ɽ")<<QStringLiteral("��˳")<<QStringLiteral("��Ϫ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("Ӫ��")<<QStringLiteral("����")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�̽�")<<QStringLiteral("��«��")<<QStringLiteral("����")<<QStringLiteral("ʯ��ׯ")<<QStringLiteral("��ɽ")<<QStringLiteral("�ػʵ�")<<QStringLiteral("����")<<QStringLiteral("��̨")<<
	QStringLiteral("����")<<QStringLiteral("�ȷ�")<<QStringLiteral("�żҿ�")<<QStringLiteral("����")<<QStringLiteral("��ˮ")<<QStringLiteral("�е�")<<QStringLiteral("����Ͽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ܿ�")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("ƽ��ɽ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("�괨")<<QStringLiteral("���")<<QStringLiteral("����")<<
	QStringLiteral("���")<<QStringLiteral("֣��")<<QStringLiteral("פ���")<<QStringLiteral("�ױ�")<<QStringLiteral("����")<<QStringLiteral("���")<<QStringLiteral("������")<<QStringLiteral("�������")<<QStringLiteral("ĵ����")<<QStringLiteral("��ľ˹")<<
	QStringLiteral("�绯")<<QStringLiteral("�ں�")<<QStringLiteral("���˰���")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("��̨��")<<QStringLiteral("����")<<QStringLiteral("�׸�")<<QStringLiteral("˫Ѽɽ")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("�ӱ�")<<QStringLiteral("��ƽ")<<QStringLiteral("ͨ��")<<QStringLiteral("�׳�")<<QStringLiteral("��Դ")<<QStringLiteral("��ԭ")<<QStringLiteral("��ɽ")<<QStringLiteral("˷��")<<QStringLiteral("����")<<

	QStringLiteral("̫ԭ")<<QStringLiteral("��ͬ")<<QStringLiteral("��Ȫ")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�ٷ�")<<QStringLiteral("����")<<QStringLiteral("�˳�")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("���ľ")<<QStringLiteral("����")<<QStringLiteral("ʯ��ɽ")<<
	QStringLiteral("����")<<QStringLiteral("��ԭ")<<QStringLiteral("����")<<QStringLiteral("���ױ���")<<QStringLiteral("���ͺ���")<<QStringLiteral("��ͷ")<<QStringLiteral("�ں�")<<QStringLiteral("�����첼")<<QStringLiteral("ͨ��")<<QStringLiteral("���")<<
	QStringLiteral("������˹")<<QStringLiteral("�����׶�")<<QStringLiteral("����")<<QStringLiteral("�˰���")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("�տ���")<<QStringLiteral("ɽ��")<<QStringLiteral("��֥")<<QStringLiteral("����")<<
	QStringLiteral("����")<<QStringLiteral("����");

QStringList uaname=QStringList()<<QStringLiteral("����")<<QStringLiteral("Ħ������")<<QStringLiteral("����")<<"LG"<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("����")<<QStringLiteral("������")
	<<QStringLiteral("����")<<QStringLiteral("��ɭ")<<QStringLiteral("����")<<QStringLiteral("��ά")<<QStringLiteral("��¼")<<QStringLiteral("������")<<QStringLiteral("��ݮ")<<QStringLiteral("ŵ����")<<"HTC"<<"TCL"
	<<QStringLiteral("UT˹�￵")<<QStringLiteral("����")<<QStringLiteral("���ΰҵ")<<QStringLiteral("��Ϊ")<<QStringLiteral("���")<<QStringLiteral("�˱�")<<QStringLiteral("������Ԫ")<<QStringLiteral("����")<<QStringLiteral("�׷�")<<"OKWAP"
	<<QStringLiteral("����")<<QStringLiteral("������")<<QStringLiteral("����")<<QStringLiteral("����")<<"NEC"<<"CECT"<<QStringLiteral("�꿵")<<QStringLiteral("ʢ̩")<<QStringLiteral("����")<<QStringLiteral("���մ�")
	<<QStringLiteral("����")<<QStringLiteral("����");

QStringList uachar=QStringList()<<"SCH"<<"MOT"<<"YL-COOLPAD"<<"LG"<<"ZTE"<<"LNV-Lenovo"<<"HX-"<<"DT-"<<"GX"<<"FCJ-"
	<<"QIGI"<<"DS-"<<"HC-"<<"CW-"<<"CHL-"<<"SAF"<<"BlackBerry"<<"NOK-"<<"HTC-"<<"TCL-"
	<<"UT-"<<"DX-"<<"HTW-"<<"HW-"<<"NAIDE-"<<"MOB-"<<"YCT-"<<"TY-"<<"YFZ"<<"OKW-"
	<<"HL-"<<"CB-"<<"TG-"<<"GIONEE"<<"NEC-"<<"CECT-"<<"HK-"<<"CKING-"<<"BD-"<<"DOPOD-"
	<<"MMX-"<<"AMOI-";

QStringList osname=QStringList()<<"android"<<"Linux"<<"windows"<<"bada";

int citynum(const char *tel) {
	int midthree;
	midthree=(tel[4]-'0')*100+(tel[5]-'0')*10+tel[6]-'0';
	//�ж����ݱ��Ƿ����
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