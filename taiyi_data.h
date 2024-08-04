#pragma once

#include <stdint.h>
#include <sys/types.h>

/// @brief 五档行情结构
struct taiyi_market_data_t
{
	uint32_t instrument_id; ///< taiyi合约id

	double last_px;		 ///< 最新价
	uint32_t last_share; ///< 最新成交量
	double total_value;	 ///< 成交金额
	double total_pos;	 ///< 持仓量

	double bid1_px;		 ///< 买一价
	uint32_t bid1_share; ///< 买一量
	double bid2_px;		 ///< 买二价
	uint32_t bid2_share; ///< 买二量
	double bid3_px;		 ///< 买三价
	uint32_t bid3_share; ///< 买三量
	double bid4_px;		 ///< 买四价
	uint32_t bid4_share; ///< 买四量
	double bid5_px;		 ///< 买五价
	uint32_t bid5_share; ///< 买五量

	double ask1_px;		 ///< 卖一价
	uint32_t ask1_share; ///< 卖一量
	double ask2_px;		 ///< 卖二价
	uint32_t ask2_share; ///< 卖二量
	double ask3_px;		 ///< 卖三价
	uint32_t ask3_share; ///< 卖三量
	double ask4_px;		 ///< 卖四价
	uint32_t ask4_share; ///< 卖四量
	double ask5_px;		 ///< 卖五价
	uint32_t ask5_share; ///< 卖五量
};
