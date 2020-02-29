#pragma once

#include <string>
#include <unordered_map>

#define OPTESTMSG	0x00
#define OPRESTART	0x01
#define OPSHUTDOWN	0x02
#define OPDISCONN	0x03
#define OPPOLL		0x10
#define OPPOLLREPLY	0x11
#define OPOUTLASER	0x21
#define OPOUTCLOSE	0x20
#define OPOUTSTRIP	0x22
#define OPOUTACK	0x2f
#define OPGETIPCONF	0x30
#define OPGETAPLIST	0x31
#define OPSETMODE	0x40
#define OPSETIPADD	0x41
#define OPSETWIFIAP	0x42
#define OPSETACK	0x4f
#define OPAPREPLY	0x52
#define OPOUTDMX256	0xc0
#define OPOUTDMX512	0xd0
#define OPOUTDMX1K	0xe0
#define OPOUTDMX2K	0xf0

class SLLCP
{
private:
	inline static std::unordered_map<int, std::string> OpCodeNames =
	{
		{OPTESTMSG, "OpTestMsg"},
		{OPRESTART, "OpRestart"},
		{OPSHUTDOWN, "OpShutdown"},
		{OPDISCONN, "OpDisconnect"},
		{OPPOLL, "OpPoll"},
		{OPPOLLREPLY, "OpPollReply"},
		{OPOUTLASER, "OpOutLaser"},
		{OPOUTCLOSE, "OpOutClose"},
		{OPOUTSTRIP, "OpOutStrip"},
		{OPOUTACK, "OpOutAck"},
		{OPGETIPCONF, "OpGetIpConf"},
		{OPGETAPLIST, "OpGetApList"},
		{OPSETMODE, "OpSetMode"},
		{OPSETIPADD, "OpSetIpAddress"},
		{OPSETWIFIAP, "OpSetWifiAp"},
		{OPSETACK, "OpSetAck"},
		{OPAPREPLY, "OpApReply"},
		{OPOUTDMX256, "OpOutDmx256"},
		{OPOUTDMX512, "OpOutDmx512"},
		{OPOUTDMX1K, "OpOutDmx1024"},
		{OPOUTDMX2K, "OpOutDmx2048"}
	};

	inline static std::unordered_map<int, std::string> OpCodeDescriptions =
	{
		{OPTESTMSG, "It's a Test message."},
		{OPRESTART, "It's a Restart command. The receiver have to restart."},
		{OPSHUTDOWN, "It's a Shutdown command. The receiver have to stop."},
		{OPDISCONN, "It's a Disconnect annotation message.\nThe sender is going offline."},
		{OPPOLL, "It's a Poll request."},
		{OPPOLLREPLY, "It's a Reply for a Poll request."},
		{OPOUTLASER, "It's a Laser data packet."},
		{OPOUTCLOSE, "It's a command. The receiver Laser must close the shutter."},
		{OPOUTSTRIP, "It's a LED strip data packet."},
		{OPOUTACK, "It's an ACK for an Output message."},
		{OPGETIPCONF, "It's a Getter. The receiver have to send the Network configuration."},
		{OPGETAPLIST, "It's a Getter. The receiver have to send a list of the seen Access Points."},
		{OPSETMODE, "It's a Setter. Changes the receivers DMX mode."},
		{OPSETIPADD, "It's a Setter. Changes the receivers Network configuration."},
		{OPSETWIFIAP, "It's a Setter. The receiver have to connect to the mentioned Access Point."},
		{OPSETACK, "It's an ACK for a Setter message."},
		{OPAPREPLY, "It's a Reply for a GETAPLIST message. Contains the seen Access Points."},
		{OPOUTDMX256, "It's a packet for one universe of DMX-256 data."},
		{OPOUTDMX512, "It's a packet for one universe of DMX-512 data."},
		{OPOUTDMX1K, "It's a packet for one universe of DMX-1024 data."},
		{OPOUTDMX2K, "It's a packet for one universe of DMX-2048 data."}
	};

public:
	static std::string getOpName(uint8_t);
	static std::string getOpDesc(uint8_t);
};