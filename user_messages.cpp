#include "user_messages.h"

void UM_SayText(int *clients, int clientsCount, int author, bool addTag, const char *input)
{
	if( !input  )
	{
		return;
	}

	static int msgId = -1;

	if( msgId == -1 )
	{
		msgId = usermsgs->GetMessageIndex("SayText2");
	}

	CCSUsrMsg_SayText2 *msg = (CCSUsrMsg_SayText2 *) usermsgs->StartProtobufMessage(msgId, clients, 1, USERMSG_RELIABLE | USERMSG_BLOCKHOOKS);

	if( msg )
	{
		// Colors
		//	x01		->		default (white)
		//	x03		->		team (blue or orange)
		//	x04		->		green
		//	x05		->		olive
		//	x07%s	->		RRGGBB
		//	x08%s	->		RRGGBBAA

		static char buffer[1024];

		if( addTag )
		{
			ke::SafeSprintf(buffer, sizeof(buffer), "\x01 \x04[\x03%s\x04] \x01%s", g_pGameMod->GetTag(), input);
		}
		else
		{
			ke::SafeSprintf(buffer, sizeof(buffer), "\x01 \x01%s", input);
		}

		msg->set_ent_idx(author);
		msg->set_chat(true);
		msg->set_msg_name(buffer);

		const google::protobuf::FieldDescriptor *field = msg->GetDescriptor()->FindFieldByName("params");

		if( field )
		{
			msg->GetReflection()->AddString(msg, field, "");
			msg->GetReflection()->AddString(msg, field, "");
			msg->GetReflection()->AddString(msg, field, "");
			msg->GetReflection()->AddString(msg, field, "");
		}
	
		usermsgs->EndMessage();		
	}
}

void UM_HudText(int *clients, int clientsCount, int channel, float posX, float posY, Color color1, Color color2, int effect, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, const char *input)
{
	if( !input )
	{
		return;
	}

	static int msgId = -1;

	if( msgId == -1 )
	{
		msgId = usermsgs->GetMessageIndex("HudMsg");
	}

	CCSUsrMsg_HudMsg *msg = (CCSUsrMsg_HudMsg *) usermsgs->StartProtobufMessage(msgId, clients, clientsCount, USERMSG_BLOCKHOOKS);

	if( msg )
	{
		msg->set_channel(channel & 0xFF);		
		
		CMsgVector2D *pos = msg->mutable_pos();
		pos->set_x(posX);
		pos->set_y(posY);

		CMsgRGBA *clr = msg->mutable_clr1();
		clr->set_r(color1.r());
		clr->set_g(color1.g());
		clr->set_b(color1.b());
		clr->set_a(color1.a());
		
		clr = msg->mutable_clr2();
		clr->set_r(color2.r());
		clr->set_g(color2.g());
		clr->set_b(color2.b());
		clr->set_a(color2.a());

		msg->set_effect(effect); 
		msg->set_fade_in_time(fadeInTime); 
		msg->set_fade_out_time(fadeOutTime);
		msg->set_hold_time(holdTime); 
		msg->set_fx_time(fxTime); 

		msg->set_text(input); 	

		usermsgs->EndMessage();			
	}	
}

void UM_ScreenFade(int *clients, int clientsCount, int duration, int holdTime, int flags, Color color)
{
	static int msgId = -1;

	if( msgId == -1 )
	{
		msgId = usermsgs->GetMessageIndex("Fade");
	}

	CCSUsrMsg_Fade *msg = (CCSUsrMsg_Fade *) usermsgs->StartProtobufMessage(msgId, clients, clientsCount, USERMSG_BLOCKHOOKS);

	if( msg )
	{
		msg->set_duration(duration);
		msg->set_hold_time(holdTime);
		msg->set_flags(flags);

		CMsgRGBA *clr = msg->mutable_clr();
		clr->set_r(color.r());
		clr->set_g(color.g());
		clr->set_b(color.b());
		clr->set_a(color.a());
			
		usermsgs->EndMessage();
	}
}

void UM_ScreenShake(int *clients, int clientsCount, int command, float amplitude, float frequency, float duration)
{
	static int msgId = -1;

	if( msgId == -1 )
	{
		msgId = usermsgs->GetMessageIndex("Shake");
	}

	CCSUsrMsg_Shake *msg = (CCSUsrMsg_Shake *) usermsgs->StartProtobufMessage(msgId, (const cell_t *) clients, clientsCount, USERMSG_BLOCKHOOKS);

	if( msg )
	{
		msg->set_command(command);
		msg->set_local_amplitude(amplitude);
		msg->set_frequency(frequency);
		msg->set_duration(duration);
		
		usermsgs->EndMessage();
	}
}
