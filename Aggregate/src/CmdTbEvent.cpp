/*******************************************************************************
 * Project:  Nebio
 * @file     CmdTbEvent.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdTbEvent.hpp"
#include "UnixTime.hpp"

namespace nebio
{

CmdTbEvent::CmdTbEvent(int32 iCmd)
   : neb::Cmd(iCmd), 
     m_uiDate(0), m_dSessionTimeout(10.0)
{
}

CmdTbEvent::~CmdTbEvent()
{
}

bool CmdTbEvent::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    oJsonConf["analyse"]["session_timeout"].Get("session_tb_event", m_dSessionTimeout);
    return(true);
}

bool CmdTbEvent::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Result oResult;
    if (oResult.ParseFromString(oMsgBody.data()))
    {
        std::ostringstream oss;
        oss << "SessionTbEvent-" << oResult.app_id() << "-" << oResult.channel() << "-" << oResult.tag() << "-" << oResult.key1();
        std::string strSessionId = oss.str();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            m_uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
            m_strDate = neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d");
            pSession = MakeSharedSession("nebio::SessionTbEvent", strSessionId, m_uiDate, m_strDate, m_dSessionTimeout);
        }
        if (pSession == nullptr)
        {
            return(false);
        }

        std::shared_ptr<SessionTbEvent> pSessionTbEvent = std::dynamic_pointer_cast<SessionTbEvent>(pSession);
        pSessionTbEvent->AddResult(oResult);
        return(true);
    }
    else
    {
        LOG4_ERROR("nebio::Result failed to parse MsgBody.data()!");
        return(false);
    }
}

}

