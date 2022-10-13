using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;

public class TestScript : MonoBehaviour
{
    #if UNITY_IOS
    const string dll = "__Internal";
    #else
    const string dll = "zoom_sdk_dotnet_wrap";
    #endif

    [Header("Off Meeting")]
    public Button authButton;
    public Button joinButton;
    public InputField meetingNumField;
    public InputField userNameField;
    public GameObject joinPanel;


    [Header("In Meeting")]
    public GameObject meetingPanel;
    public Button leaveButton;
    public Button retrVideoButton;
    public Button toggleVideoButton;
    public Button toggleAudioButton;
    public Text loading;
    public Dropdown options;

    [Header("General Authentication")]
    public string jwtToken;
    public bool useDefaultJWTToken; //the default token expires on 11/7/2022

    //Test function to see if DLL is connected. Returns 100 on success
    [DllImport(dll)]
    private static extern int Tester();

    // initialize SDK at start
    [DllImport(dll)]
    private static extern int Init();

    //clean up SDK related functions. To be called at the end of meetings
    [DllImport(dll)]
    public static extern void CleanUp();

    // authorization
    [DllImport(dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern int GetAuth(string token, bool useDefault);
    [DllImport(dll)]
    private static extern int GetAuthCode(); //returns 0 on successful authorization

    // retrieves meeting status
    [DllImport(dll)]
    private static extern int GetMeetingCode();

    // join
    [DllImport(dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern int Join(long num, string meetingName);
   
    // leave
    [DllImport(dll)]
    private static extern int Leave();
    
    // retrieves video feed of other users and adds it to the window specified.
    //Note: Unity seems to have special ways of handling their own windows,
    //so it does not seem possible to add videos on top of Unity windows for now
    [DllImport(dll)]
    private static extern int RetrieveVideo(IntPtr handle);
    
    // Functions to check various statuses
    [DllImport(dll)]
    private static extern bool isSelfHost();
    [DllImport(dll)]
    private static extern bool isVideoOn();
    [DllImport(dll)]
    private static extern bool isAudioOn();
    
    // turns mic on/off
    [DllImport(dll)]
    private static extern void ToggleAudio(bool resp); // true = unmute, false = mute

    // turns on/off video camera
    [DllImport(dll)]
    private static extern void ToggleVideo(bool resp); // true = turn on video, false = turn off video

    
    // gets list of cameras user has on their device and allows them to change their camera
    [DllImport(dll)]
    private static extern int GetCamerasCount();
    [DllImport(dll)]
    private static extern IntPtr GetCameraName(int pos);
    [DllImport(dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void SelectNewCam(int pos);
    
    // Initializing and retrieving window handle
    private System.IntPtr ptr = System.IntPtr.Zero;

    // functions to spawn new window and close them
    [DllImport("DLL4")]
    static extern int Initalize([MarshalAs(UnmanagedType.LPStr)]string windowName);
    [DllImport("DLL4")]
    static extern void Shutdown();

    // functions relating to getting a window handle and changing their position and size
    [System.Runtime.InteropServices.DllImport("user32.dll")]
    private static extern System.IntPtr GetActiveWindow();
    [System.Runtime.InteropServices.DllImport("user32.dll")]
    private static extern System.IntPtr SetWindowPos(System.IntPtr hWnd, int hWndInsertAfter, int X, int Y, int cx, int cy, int uFlags);

    [Header("Other miscellanous variables")]
    public string windowName;
    private bool checkingAuth;
    private bool authComplete;
    private bool authorized = false;

    private void Start()
    {
        checkingAuth = false;
        authComplete = false;

        // connect buttons to on click events
        authButton.onClick.AddListener(authorize);
        joinButton.onClick.AddListener(join);
        leaveButton.onClick.AddListener(leave);
        
        retrVideoButton.onClick.AddListener(retrieveVideo);
        toggleVideoButton.onClick.AddListener(toggleVideo);
        toggleAudioButton.onClick.AddListener(toggleAudio);

        options.onValueChanged.AddListener(delegate
        {
            CameraChange();
        });

        if (Tester() == 100){
            Debug.Log("DLL is connected");
        }


    }

  

    private void retrieveVideo(){
        //creates new window to spawn video feeds on top of it
        Initalize(windowName);
        ptr = GetActiveWindow();
        int vidRes = RetrieveVideo(ptr);

        Debug.Log("retrieving video...");
        Debug.Log("Number of participants: " + vidRes);
        
        //flag for setWindowPos below is SWP_NOMOVE which makes sure the window's position does not change after resizing
        SetWindowPos(ptr, 0, 0, 0, 900, 300 * (int)(Math.Ceiling(vidRes / 3.00)) + 50, 0x0002); 
    }

    private void toggleVideo(){
        if(!isVideoOn()){
            ToggleVideo(true);
        }else{
            ToggleVideo(false);
        }  
    }

    private void toggleAudio(){
        if(!isAudioOn()){
            ToggleAudio(true);
        }else{
            ToggleAudio(false);
        }  
    }

    // ------- SDK Function Wrappers ---------
    private void authorize(){
        // initialize
        Debug.Log("Init: " + getSDKError(Init()));
        // authorize
        Debug.Log("Auth: " + getSDKError(GetAuth(jwtToken, useDefaultJWTToken)));
        // get meeting status
        Debug.Log(translateMC(GetMeetingCode()));
        checkingAuth = true;
        loading.gameObject.SetActive(true);
    }

    private void join(){
        long meetingNum;
        string userName;

        if(!meetingNumField.text.Equals("")){
            meetingNum = long.Parse(meetingNumField.text);
        }
        else
        {
            meetingNum = 0;
        }

        if(!userNameField.text.Equals("")){
            userName = userNameField.text;
        }else{
            userName = "Unity user";
        }

        if (authComplete)
        {
            if (meetingNum != 0)
            {
                Join(meetingNum, userName);
                authorized = true;
            }
            else
            {
                Debug.Log("meeting field is blank.");
            }
            
        }
        else
        {
            Debug.Log("cannot join rooms right now. Not authenticated");
        }
       
    }

    private void leave(){
        Leave();
        meetingPanel.SetActive(false);
        Shutdown();
    }

    private void CameraChange()
    {
        SelectNewCam(options.value);
    }

    void Update(){

        //checks for when the user has been successfully authenticated
        if (checkingAuth)
        {
            if(GetAuthCode() == 0)
            {
                authComplete = true;
                checkingAuth = false;
                Debug.Log("can now join rooms");
                loading.gameObject.SetActive(false);
                joinPanel.SetActive(true);
            }
        }
        
        
        if(authorized){
            //prints meeting status
            Debug.Log(translateMC(GetMeetingCode()));

            if(GetMeetingCode() == 3){ //if successfully joined meeting
                meetingPanel.SetActive(true);

                //populates list of camera devices
                int cameraCount = GetCamerasCount();
                for (int i = 0; i < cameraCount; i++)
                {
                    Dropdown.OptionData cameras = new Dropdown.OptionData();
                    cameras.text = Marshal.PtrToStringAnsi(GetCameraName(i));
                    options.options.Add(cameras);
                }


                authorized = false;
            }
        }


    }

    void OnDestroy()
    {
        
        CleanUp();
        Shutdown();
        
    }


    // ---------- Helper Functions ------------
    private string getSDKError(int val){
        if(val == 0){
            return "SUCCESSSSSSS";
        }

        string output = "Error::";

        switch(val) 
        {
        case 1:
            output += "Not support this feature now";
            break;
        case 2:
            output += "Wrong useage about this feature";
            break;
        case 3:
            output += "Wrong parameter";
            break;
        case 4:
            output += "Load module failed";
            break;
        case 5:
            output += "No memory allocated";
            break;
        case 6:
            output += "Internal service error";
            break;
        case 7:
            output += "Not initialize before use";
            break;
        case 8:
            output += "Success Result";
            break;
        case 9:
            output += "Not Authentication before use";
            break;
        case 10:
            output += "No recording in process";
            break;
        case 11:
            output += "can't find transcoder module";
            break;
        case 12:
            output += "Video service not ready";
            break;
        case 13:
            output += "No premission to do this";
            break;
        case 14:
            output += "Unknown error";
            break;
        case 15:
                output += "Other SDK instance running/JWT expired";
                break;
        default:
            output = "null";
            break;
        }

        return output;
    }

    private string translateMC(int val){

            string output = "Meeting status code: ";

            switch(val) 
            {
            case 0:
                output += "Idle status, no meeting running";
                break;
            case 1:
                output += "Connecting meeting server status";
                break;
            case 2:
                output += "Waiting for host to start meeting";
                break;
            case 3:
                output += "Meeting is ready, in meeting status";
                break;
            case 4:
                output += "Disconnecting meeting server status";
                break;
            case 5:
                output += "Reconnecting meeting server status";
                break;
            case 6:
                output += "Meeting connection error";
                break;
            case 7:
                output += "Meeting has ended";
                break;
            case 8:
                output += "Meeting status unknown";
                break;
            case 9:
                output += "Meeting is locked";
                break;
            case 10:
                output += "Meeting is unlocked";
                break;
            case 11:
                output += "Waiting in waiting room";
                break;
            case 12:
                output += "Webinar promote";
                break;
            case 13:
                output += "Webinar depromote";
                break;
            case 14:
                output += "Joining breakout room";
                break;
            case 15:
                output += "Leaving breakout room";
                break;
            case 16:
                output += "Waiting for external session key";
                break;
            default:
                output = "null";
                break;
            }

            return output;
    }

    


}