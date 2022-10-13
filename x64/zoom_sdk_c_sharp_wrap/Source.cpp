#include "pch.h"
#include "stdafx.h"
#include "zoom_sdk_dotnet_wrap.h"
#include "zoom_sdk_dotnet_wrap_util.h"
#include "wrap/sdk_wrap.h"

#define DllExport __declspec(dllexport)

namespace ZOOM_SDK_DOTNET_WRAP {

	static int authCode = -1;
	static int meetingCode = -1;
	static int recordingCode = -1;
	static gcroot <array<unsigned int>^> participantList;
	static unsigned int myUserID = -1;

	extern "C" {

		DllExport int Tester() {
			return 100;
		}

		//methods to initialize and clean up SDK before and after use respectively

		DllExport int Init() {
			InitParam param = InitParam();
			param.web_domain = "https://zoom.us";
			param.config_opts.optionalFeatures = (1 << 5); // tells the sdk not to show zoom's default UI
			SDKError err = CZoomSDKeDotNetWrap::Instance->Initialize(param);
			
			return (int)err;
		}

		DllExport void CleanUp() {

			CZoomSDKeDotNetWrap::Instance->CleanUp();
		}


		//Get various status codes methods
		
		DllExport int GetAuthCode() {
			return authCode;
		}

		DllExport int GetMeetingCode() {
			return meetingCode;
		}

		DllExport unsigned int GetMyUserID() {
			if (myUserID != -1) { // if user ID has already been initialized, return it
				return myUserID;
			}
			else {
				array<unsigned int >^ userIDs = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->GetParticipantsList();
				int userCount = userIDs->Length;
				for (int i = 0; i < userCount; i++) {
					IUserInfoDotNetWrap^ participant = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->GetUserByUserID(i);
					if (participant->IsMySelf()) {
						myUserID = participant->GetUserID();
						return myUserID;
					}
				}
			}
		}

		//various callbacks

		void AuthReturn(AuthResult ret) {
			authCode = (int)ret;
		}
		
		void MeetingStatusCB(MeetingStatus status, int iResult) {
			meetingCode = (int)status;
		}

		void UserJoinCB(array<unsigned int>^ userIds) {
			participantList = userIds;
		}

		//methods relating to functionalities

		DllExport int GetAuth(const char* token, bool useDefault) {
			//create callback for authentication
			String^ jwtToken = gcnew String(token);
			
			onAuthenticationReturn^ authFunc = gcnew onAuthenticationReturn(AuthReturn);
			CZoomSDKeDotNetWrap::Instance->GetAuthServiceWrap()->Add_CB_onAuthenticationReturn(authFunc);
			
			SDKError result;

			//authorize JWT token
			if (!useDefault) {
				AuthContext param{
					param.jwt_token = jwtToken

				};

				result = CZoomSDKeDotNetWrap::Instance->GetAuthServiceWrap()->SDKAuth(param);
			}
			else {
				AuthContext param{ //place default JWT token here. The project currently does have a default one that expires in Nov 2022, but that token will not be included in this script
					param.jwt_token = ""

				};

				result = CZoomSDKeDotNetWrap::Instance->GetAuthServiceWrap()->SDKAuth(param);

			}
			return (int)result;

		}

		DllExport int Join(long long meetingNum, const char* userName) {
			//user join information
			JoinParam4WithoutLogin join_param = JoinParam4WithoutLogin();
			join_param.meetingNumber = meetingNum;
			
			String^ userName_str = gcnew String(userName);
			join_param.userName = userName_str;

			JoinParam param = JoinParam();
			param.userType = SDKUserType::SDK_UT_WITHOUT_LOGIN;
			param.withoutloginJoin = join_param;

			//create callback for when meeting status code changes
			onMeetingStatusChanged^ meetingChangeCb = gcnew onMeetingStatusChanged(MeetingStatusCB);
			CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->Add_CB_onMeetingStatusChanged(meetingChangeCb);
			
			onUserJoin^ userJoinCb = gcnew onUserJoin(UserJoinCB);
			CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->Add_CB_onUserJoin(userJoinCb);

			//join meeting
			SDKError err = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->Join(param);

			return (int)err;
		}

		DllExport int Leave() {
			// leave meeting
			SDKError err = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->Leave(LeaveMeetingCmd::LEAVE_MEETING);

			// destroy all video elements
			CCustomizedUIMgrDotNetWrap::Instance->DestroyAllVideoContainer();

			return (int)err;
		}

		
		DllExport int RetrieveVideo(int handle) {

			// create/pass in a window handle

			IntPtr winHandle =  IntPtr(handle);
			::RECT rect;
			rect.left = 0;
			rect.right = 300;
			rect.bottom = 300;
			rect.top = 0;

			//GetWindowRect(static_cast<HWND>(winHandle.ToPointer()), (&rect));
			RECT rct;

			rct.Left = rect.left;
			rct.Bottom = rect.bottom;
			rct.Right = rect.right;
			rct.Top = rect.top;

			IntPtr ptr = (IntPtr)handle;
			
			// Create video container

			ICustomizedVideoContainerDotNetWrap^ container = CZoomSDKeDotNetWrap::Instance->GetCustomizedUIMgrWrap()->CreateVideoContainer(winHandle, rct);
			SDKError err = container->Show();
			
			// create video elements
			
			// --preview video--
			//IPreviewVideoRenderElementDotNetWrapImpl^ prevVid = dynamic_cast<IPreviewVideoRenderElementDotNetWrapImpl^>(container->CreateVideoElement(VideoRenderElementType::VideoRenderElement_PREVIEW));
			//err = prevVid->Start();
			//if (err != SDKError::SDKERR_SUCCESS) {
			//	return -1;
			//}
			
			// --active video--

			//IActiveVideoRenderElementDotNetWrapImpl^ activeVid = dynamic_cast<IActiveVideoRenderElementDotNetWrapImpl^>(container->CreateVideoElement(VideoRenderElementType::VideoRenderElement_ACTIVE));
			//activeVid->EnableShowScreenNameOnVideo(true);
			//RECT pos;
			//pos.Left = 300;
			//pos.Right = 300;
			//pos.Top = 0;
			//pos.Bottom = 600;
			//activeVid->SetPos(pos);

			//err = activeVid->Start();
			//err = activeVid->Show();
			
			// --normal video--
			
			array<unsigned int >^ userIDs = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->GetParticipantsList();
			int userCount = userIDs->Length;
			int j = 0;
			

			for (int i = 0; i < userCount; i++) {
				INormalVideoRenderElementDotNetWrapImpl^ normalVid = dynamic_cast<INormalVideoRenderElementDotNetWrapImpl^>(container->CreateVideoElement(VideoRenderElementType::VideoRenderElement_NORMAL));
				
				normalVid->EnableShowScreenNameOnVideo(true);

				RECT pos;
				pos.Left = 0 + 300*(i%3);
				pos.Right = 300 + 300 * (i%3);
				pos.Top = 0 + 300 * j;
				pos.Bottom = 300 + 300 * j;
				normalVid->SetPos(pos);

				err = normalVid->Subscribe(userIDs[i]);
				err = normalVid->Show();
				if (i % 3 == 0 && i != 0) {
					j++;
				}
			}

			RECT newSize;
			newSize.Top = 0;
			newSize.Left = 0;
			newSize.Right = 300 * 3;
			newSize.Bottom = 300 * (ceil(userCount / 3.00));
			container->Resize(newSize);
			if (err != SDKError::SDKERR_SUCCESS) {
				return -3;
			}
			return userCount;
		}



		// functions for self

		DllExport void ToggleAudio(bool resp) {
			IMeetingAudioControllerDotNetWrap^ audio = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingAudioController();

			if (resp == true) {
				audio->UnMuteAudio(GetMyUserID());
			}
			else {
				audio->MuteAudio(GetMyUserID(), true);
			}
		}

		DllExport bool isAudioOn() {
			IUserInfoDotNetWrap^ self = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->GetUserByUserID(0);
			if (!self->IsAudioMuted()) {
				return true;
			}
			else {
				return false;
			}
		}

		DllExport void ToggleVideo(bool resp) {
			IMeetingVideoControllerDotNetWrap^ video = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingVideoController();
			if (resp == true) {
				video->UnmuteVideo();
			}
			else {
				video->MuteVideo();
			}
		}

		DllExport bool isVideoOn() {
			IUserInfoDotNetWrap^ self = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->GetUserByUserID(0);
			if (self->IsVideoOn()) {
				return true;
			}
			else {
				return false;
			}
		}

		DllExport bool isSelfHost() {
			IUserInfoDotNetWrap^ self = CZoomSDKeDotNetWrap::Instance->GetMeetingServiceWrap()->GetMeetingParticipantsController()->GetUserByUserID(0);
			if (self->IsHost()) {
				return true;
			}
			else {
				return false;
			}
		}
		
		//functions relating to changing cameras

		DllExport int GetCamerasCount() {
			array<ICameraInfoDotNetWrap^ >^ cameras = CZoomSDKeDotNetWrap::Instance->GetSettingServiceWrap()->GetVideoSettings()->GetCameraList();
			int length = cameras->Length;
			return length;
		}

		DllExport const char* GetCameraName(int pos) {
			String^ name = CZoomSDKeDotNetWrap::Instance->GetSettingServiceWrap()->GetVideoSettings()->GetCameraList()[pos]->GetDeviceName();
			const char* str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
			return str;
		}

		DllExport void SelectNewCam(int pos) {
			CZoomSDKeDotNetWrap::Instance->GetSettingServiceWrap()->GetVideoSettings()->SelectCamera(CZoomSDKeDotNetWrap::Instance->GetSettingServiceWrap()->GetVideoSettings()->GetCameraList()[pos]->GetDeviceId());
		}
	}



}

