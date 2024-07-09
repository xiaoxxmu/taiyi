#include "ctpapi.h"
#include "mdspi.h"
#include "tradespi.h"
#include "common.h"
#include "log.h"

TThostFtdcBrokerIDType g_broker_id = "8080";                // 经纪商代码
TThostFtdcInvestorIDType g_invester_id = "10273";           // 投资者账户名
TThostFtdcPasswordType g_invester_pwd = "123456";         // 投资者密码
//TThostFtdcInvestorIDType g_invester_id = "07420";           // 投资者账户名
//TThostFtdcPasswordType g_invester_pwd = "82537410";         // 投资者密码
TThostFtdcAuthCodeType g_auth_code = "xxx";					// 认证码
TThostFtdcAppIDType g_app_id = "client_taiyi_1.0.0";

char g_md_front_addr[] = "tcp://121.37.80.177:20004";      // 模拟行情前置地址
char g_trade_front_addr[] = "tcp://121.37.80.177:20002";   // 模拟交易前置地址

//char g_md_front_addr[] = "tcp://116.239.24.112:20006";      // 模拟行情前置地址
//char g_trade_front_addr[] = "tcp://116.239.24.112:20000";   // 模拟交易前置地址

char* g_pp_md_instrument_id[] = {"zn2409"};              	// 行情合约代码列表
uint32_t g_md_instrument_cnt = 1;                         	// 行情合约订阅数量
char* g_pp_trade_instrument_id[] = {"zn2409"};				// 交易合约代码列表
uint32_t g_trade_instrument_cnt = 1;						// 交易合约数量

CThostFtdcMdApi* g_p_md_user_api = nullptr;
TaiyiMdSpi* g_p_md_user_spi = nullptr;
CThostFtdcTraderApi* g_p_trader_api = nullptr;
TaiyiTraderSpi* g_p_trader_spi = nullptr;


TThostFtdcFrontIDType g_trade_front_id = 0;
TThostFtdcSessionIDType g_session_id = 0;
uint32_t g_order_ref = 0;

void SetTradeFrontId(TThostFtdcFrontIDType trade_front_id) {
	g_trade_front_id = trade_front_id;
}

TThostFtdcFrontIDType GetTradeFrontId() {
	return g_trade_front_id;
}

void SetSessionId(TThostFtdcSessionIDType session_id) {
	if (g_session_id < session_id) {
		g_session_id = session_id;
	}
}

TThostFtdcSessionIDType GetSessionId() {
	return g_session_id;
}

void SetOrderRef(uint32_t order_ref) {
	g_order_ref = order_ref;
}

uint32_t GetOrderRef() {
	return g_order_ref++;
}

void Start() {
	LOG_INFO("Init md thread, front addr (%s)", g_md_front_addr);
	g_p_md_user_api = CThostFtdcMdApi::CreateFtdcMdApi();
	g_p_md_user_spi = new TaiyiMdSpi;
	g_p_md_user_api->RegisterSpi(g_p_md_user_spi);
	g_p_md_user_api->RegisterFront(g_md_front_addr);
	g_p_md_user_api->Init();

	return;

	LOG_INFO("Init trader thread, front addr (%s)", g_trade_front_addr);
    g_p_trader_api = CThostFtdcTraderApi::CreateFtdcTraderApi();
	g_p_trader_spi = new TaiyiTraderSpi;
	g_p_trader_api->RegisterSpi(g_p_trader_spi);
	g_p_trader_api->SubscribePublicTopic(THOST_TERT_QUICK);
	g_p_trader_api->SubscribePrivateTopic(THOST_TERT_QUICK);
	g_p_trader_api->RegisterFront(g_trade_front_addr);
	g_p_trader_api->Init();
}

void Wait() {
    // 等到线程退出
	g_p_md_user_api->Join();
	delete g_p_md_user_spi;
	g_p_md_user_api->Release();

	return;

	g_p_trader_api->Join();
	delete g_p_trader_spi;
	g_p_trader_api->Release();
}
