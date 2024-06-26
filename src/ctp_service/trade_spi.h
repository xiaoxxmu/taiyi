/*
 * trade_spi.h
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#pragma once


#include "ThostFtdcTraderApi.h"

#include "config.h"
#include "message.h"
#include "mem.h"

class TaiyiTraderSpi : public CThostFtdcTraderSpi {
  public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	void OnHeartBeatWarning(int nTimeLapse);

	///客户端认证响应
	void OnRspAuthenticate(
		CThostFtdcRspAuthenticateField *pRspAuthenticateField,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///登录请求响应
	void OnRspUserLogin(
		CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///错误应答
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	void OnRspUserLogout(
		CThostFtdcUserLogoutField *pUserLogout,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(
		CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///请求查询合约响应
	void OnRspQryInstrument(
		CThostFtdcInstrumentField *pInstrument,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///请求查询资金账户响应
	void OnRspQryTradingAccount(
		CThostFtdcTradingAccountField *pTradingAccount,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(
		CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///报单录入请求响应
	void OnRspOrderInsert(
		CThostFtdcInputOrderField *pInputOrder,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	///报单操作请求响应
	void OnRspOrderAction(
		CThostFtdcInputOrderActionField *pInputOrderAction,
		CThostFtdcRspInfoField *pRspInfo,
		int nRequestID,
		bool bIsLast);

	///报单通知
	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	void OnRtnTrade(CThostFtdcTradeField *pTrade);

public:
    TaiyiTraderSpi(Config* cfg, CThostFtdcTraderApi* pTraderApi);
    ~TaiyiTraderSpi() {} // TODO

  private:
    CThostFtdcTraderApi* _pTraderApi;

  private:
    uint32_t _containerId;
    uint32_t _moduleId;

  private:
	TThostFtdcBrokerIDType _brokerId;
	TThostFtdcUserIDType _investerId;
	TThostFtdcPasswordType _investerPwd;
	TThostFtdcAuthCodeType	_authCode;
	TThostFtdcAppIDType	_appId;

  private:
    // 会话参数
    TThostFtdcFrontIDType _tradeFrontId; //前置编号
    TThostFtdcSessionIDType _sessionId; //会话编号
    TThostFtdcOrderRefType _orderRef; //报单引用
    time_t _lOrderTime;
    time_t _lOrderOkTime;

  private:
	TaiyiMemPool* _pMsgPool;
	TaiyiMemPool* _pOrderInsertRspPool;
};
