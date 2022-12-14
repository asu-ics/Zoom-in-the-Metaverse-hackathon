/*!
* \file meeting_chat_interface.h
* \brief Meeting Service Chat Interface. 
* \remarks Valid for both ZOOM style and user custom interface mode.
*/
#ifndef _MEETING_CHAT_INTERFACE_H_
#define _MEETING_CHAT_INTERFACE_H_
#include "zoom_sdk_def.h"

BEGIN_ZOOM_SDK_NAMESPACE
/**
 * @brief Enumerations of the type for chat message.
 */
typedef enum
{
	SDKChatMessageType_To_None, ///For initialize	
	SDKChatMessageType_To_All,///Chat message is send to all.	
	SDKChatMessageType_To_All_Panelist,///Chat message is send to all panelists.	
	SDKChatMessageType_To_Individual_Panelist,///Chat message is send to individual attendee and cc panelists.	
	SDKChatMessageType_To_Individual,///Chat message is send to individual user.	
	SDKChatMessageType_To_WaitingRoomUsers,///Chat message is send to waiting room user.
}SDKChatMessageType;

/// \brief Chat message interface.
///
class IChatMsgInfo
{
public:
	/// \brief Get the message ID of the current message.
	/// \return If the function succeeds, the return value is the message ID of the current message.
	///Otherwise failed, the return value is the string of length zero(0)
	virtual const wchar_t* GetMessageID() = 0;

	/// \brief Get the sender ID of the current message.
	/// \return If the function succeeds, the return value is the sender ID of the current message.
	///Otherwise failed, the return value is -1.
	virtual unsigned int GetSenderUserId() = 0;

	/// \brief Get the sender screen name of the current message.
	/// \return If the function succeeds, the return value is sender screen name of the current message.
	///Otherwise failed, the return value is NULL.
	/// \remarks If the message is sent to all or to all panelists, the return value will be NULL.
	virtual const wchar_t* GetSenderDisplayName() = 0;

	/// \brief Get the receiver ID of the current message.
	/// \return If the function succeeds, the return value is the receiver ID of the current message.
	///ZERO(0) indicates that the message is sent to all.
	///ONE(1) indicates that the messages are sent to all the panelists.
	///Otherwise failed, the return value is negative ONE(-1).
	virtual unsigned int GetReceiverUserId() = 0;

	/// \brief Get the receiver screen name of the current message.
	/// \return If the function succeeds, the return value is the receiver screen name of the current message.
	///Otherwise failed, the return value is the string of length zero(0).
	virtual const wchar_t* GetReceiverDisplayName() = 0;

	/// \brief Get the content of the current message.
	/// \return If the function succeeds, the return value is the pointer to the content of the current message.
	///Otherwise failed, the return value is NULL.
	virtual const wchar_t* GetContent() = 0;

	/// \brief Get the timestamps of the current message.
	/// \return If the function succeeds, the return value is the timestamps of the current message. 
	virtual time_t GetTimeStamp() = 0;

	/// \brief Determine if the current message is sent to all. 
	/// \return TRUE indicates that the current message is sent to all. Otherwise not. 
	virtual bool IsChatToAll() = 0;

	/// \brief Determine if the current message is sent to all the panelists.
	/// \return TRUE indicates that the current message is sent to all the panelists. Otherwise not. 
	virtual bool IsChatToAllPanelist() = 0;

	/// \brief Determine if the current message is sent to waiting room.
	/// \return TRUE indicates that the current message is sent to waiting room. Otherwise not.
	virtual bool IsChatToWaitingroom() = 0;

	/// \brief Get the chat message type of the current message.
	/// \return If the function succeeds, the return value is the chat message type of the current message.
	virtual SDKChatMessageType GetChatMessageType() = 0;

	virtual ~IChatMsgInfo() {};
};

/*! \struct NormalMeetingChatStaus
    \brief The authority to chat in the normal meeting.  
    Here are more detailed structural descriptions..
*/
typedef struct tagNormalMeetingChatStaus
{
	bool can_chat;///<TRUE indicates that the user owns the authority to send message to chat.
	bool can_chat_to_all;///<TRUE indicates that the user owns the authority to send message to all.
	bool can_chat_to_individual;///<TRUE indicates that the user owns the authority to send message to an individual attendee in the meeting.
	bool is_only_can_chat_to_host;///<TRUE indicates that the user owns the authority to send message only to the host. 
}NormalMeetingChatStaus;

/*! \struct tagWebinarAttendeeChatStatus
    \brief The authority to chat for the normal attendee in the webinar.
    Here are more detailed structural descriptions..
*/
typedef struct tagWebinarAttendeeChatStatus
{
	bool can_chat;///<TRUE indicates that the attendee can send message to chat. 
	bool can_chat_to_all_panellist_and_attendee;///<TRUE indicates that the user owns the authority to send message to all the panelists and attendees.
	bool can_chat_to_all_panellist;///<TRUE indicates that the user owns the authority to send message to all the panelists.
}WebinarAttendeeChatStatus;

/*! \struct tagWebinarOtherUserRoleChatStatus
    \brief The authority to chat for the host, co-host and panelist to chat in webinar.
    Here are more detailed structural descriptions..
*/
typedef struct tagWebinarOtherUserRoleChatStatus
{
	bool can_chat_to_all_panellist;///<TRUE indicates that the user owns the authority to send message to all the panelists.
	bool can_chat_to_all_panellist_and_attendee;///<TRUE indicates that the user owns the authority to send message to all.
	bool can_chat_to_individual;///<TRUE indicates that the user owns the authority to send message to individual attendee.
}WebinarOtherUserRoleChatStatus;

/*! \struct tagChatStatus
    \brief The authority to chat in the specified meeting.
    Here are more detailed structural descriptions..
*/
typedef struct tagChatStatus
{
	union
	{
		NormalMeetingChatStaus normal_meeting_status;
		WebinarAttendeeChatStatus webinar_attendee_status;
		WebinarOtherUserRoleChatStatus webinar_other_status;
	}ut;///<The ut value depends on the value of the other members in the structure. When the value of is_webinar_meeting is false, the ut value is the NormalMeetingChatStausnormal_meeting_status. When the values of the is_webinar_meeting and the is_webinar_attendee is true, the ut value is WebinarAttendeeChatStatus webinar_attendee_status. The value of is_webinar_meeting is true while the is_webinar_attendee is false, the ut value is WebinarOtherUserRoleChatStatus webinar_other_status.
	bool is_chat_off;///<TRUE indicates that it is disabled to chat in the specified meeting. 
	bool is_webinar_attendee;///<TRUE indicates that the owner of the current message is the attendee of the webinar. 
	bool is_webinar_meeting;///<TRUE indicates that the current meeting is webinar.

	tagChatStatus()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(tagChatStatus));  //checked safe
	}
}ChatStatus;

/**
 * @brief Enumerations of the chat priviledge.
 */
typedef enum {
	SDK_CHAT_PRIVILEDGE_ALL = 1,                    	/// allow attendee to chat with everyone [meeting & webinar]
	SDK_CHAT_PRIVILEDGE_ALL_PANELIST = 2,		          /// allow attendee to chat with all panelists only, but cannot to "all panelists and attendees" [webinar]
	SDK_CHAT_PRIVILEDGE_HOST = 3,	                    /// allow attendee to chat with host only [meeting]
	SDK_CHAT_PRIVILEDGE_DISABLE_ATTENDEE_CHAT = 4,    /// allow attendee to chat with no one [meeting & webinar]
	SDK_CHAT_PRIVILEDGE_HOST_PUBLIC = 5,              /// allow attendee to chat with host and public [meeting]
	SDK_CHAT_PRIVILEDGE_END
} SDKChatPriviledge;

/**
 * @brief Enumerations of the chat message delete type.
 */
typedef enum
{
	SDK_CHAT_DELETE_BY_NONE,	/// none
	SDK_CHAT_DELETE_BY_SELF,	/// delete by self
	SDK_CHAT_DELETE_BY_HOST,	/// delete by host
	SDK_CHAT_DELETE_BY_DLP,		/// delete by dlp when the message goes against the host organization's compliance policies.
}SDKChatMessageDeleteType;

/// \brief Meeting chat callback event.
///
class IMeetingChatCtrlEvent
{
public:
	virtual ~IMeetingChatCtrlEvent() {}

	/// \brief Chat message callback. This function is used to inform the user once received the message sent by others.
	/// \param chatMsg An object pointer to the chat message.
	/// \param content A pointer to the chat message in json format. This parameter is currently invalid, hereby only for reservations. 
	virtual void onChatMsgNotifcation(IChatMsgInfo* chatMsg, const wchar_t* content = NULL) = 0;

	/// \brief The authority of chat changes callback. This function is used to inform the user when the authority of chat changes in the meeting or webinar.
	/// \param status_ The chat status. For more details, see \link ChatStatus \endlink.
	virtual void onChatStautsChangedNotification(ChatStatus* status_) = 0;

	/// \brief Chat message be deleted callback. This function is used to inform the user host/myself the message be deleted.
	/// \param MsgID is the id of the deleted message.
	/// \param deleteBy Indicates by whom the message was deleted.
	virtual void onChatMsgDeleteNotification(const wchar_t* msgID, SDKChatMessageDeleteType deleteBy) = 0;
};

/// \brief Meeting chat controller interface
///
class IMeetingChatController
{
public:
	/// \brief Set meeting chat callback event.
	/// \param pEvent A pointer to the IMeetingChatCtrlEvent to receive chat callback event. For more details, see \link IMeetingChatCtrlEvent \endlink.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	/// \remark The event is used by SDK to pass the callback event to user's application. If this function is not called or fails, the user's application can not retrieve the callback event.
	virtual SDKError SetEvent(IMeetingChatCtrlEvent* pEvent) = 0;

	/// \brief Get the authority status to send current message. 
	/// \return If the function succeeds, the return value is a pointer to the structure of ChatStatus. For more details, see \link ChatStatus \endlink structure.
	///Otherwise failed, the return value is NULL. To get extended error information, see \link ChatStatus \endlink.
	virtual const ChatStatus* GetChatStatus() = 0;

	/// \brief Set the chat priviledge of participants.
	/// \param priviledge The chat priviledge of participants
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SetParticipantsChatPriviledge(SDKChatPriviledge priviledge) = 0;

	/// \brief Send chat message in the normal meeting.
	/// \param receiver Specify the user ID who receives the chat message. The message will be sent to all when the value is zero(0). 
	/// \param content The content of the chat message. 
	/// \param type The type of the chat message
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError SendChatMsgTo(wchar_t* content, unsigned int receiver, SDKChatMessageType type) = 0;

	/// \brief Determine whether the legal notice for chat is available
	/// \return True indicates the legal notice for chat is available. Otherwise False.
	virtual bool IsMeetingChatLegalNoticeAvailable() = 0;

	/// Get the chat legal notices prompt.
	virtual const wchar_t* getChatLegalNoticesPrompt() = 0;

	/// Get the chat legal notices explained.
	virtual const wchar_t* getChatLegalNoticesExplained() = 0;

	/// \brief Determine whether the message can be delete.
	/// \param msgID is the message id.
	/// \return True indicates the message can be delete. Otherwise False.	
	virtual bool IsChatMessageCanBeDeleted(const wchar_t* msgID) = 0;

	/// Delete chat message by message id.	
	/// \param msgID is the message id.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link SDKError \endlink enum.
	virtual SDKError DeleteChatMessage(const wchar_t* msgID) = 0;

	/// Get all chat message id.	
	virtual IList<const wchar_t*>* GetAllChatMessageID() = 0;
};
END_ZOOM_SDK_NAMESPACE
#endif