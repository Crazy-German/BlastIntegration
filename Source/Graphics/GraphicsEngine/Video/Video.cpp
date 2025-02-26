#include "Video.h"
#include "Texture.h"
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wmcodecdsp.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

#define CHECK_HRESULT(x, msg) hr = x; if( hr != S_OK ) { /*debug?*/ return false; }
#define SAFE_RELEASE(x) if(x) { (x)->Release(); x = nullptr; }

#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#endif

LPCSTR GetGUIDNameConst(const GUID & guid)
{
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
    IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
    IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
    IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
    IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
    IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
    IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
    IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
    IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
    IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
    IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
    IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
    IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
    IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
    IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

    // Media types

    IF_EQUAL_RETURN(guid, MFMediaType_Audio);
    IF_EQUAL_RETURN(guid, MFMediaType_Video);
    IF_EQUAL_RETURN(guid, MFMediaType_Protected);
    IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
    IF_EQUAL_RETURN(guid, MFMediaType_Script);
    IF_EQUAL_RETURN(guid, MFMediaType_Image);
    IF_EQUAL_RETURN(guid, MFMediaType_HTML);
    IF_EQUAL_RETURN(guid, MFMediaType_Binary);
    IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

    IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
    IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
    IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
    IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
    IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
    IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
    IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

    IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
    IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
    IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

    return NULL;
}

#define CHECKHR_GOTO(x,y) if(x != S_OK) goto y;

//// Returns all attributes from the stream in string format
//std::string GetMediaTypeDescription(IMFMediaType * pMediaType)
//{
//    HRESULT hr = S_OK;
//    GUID MajorType;
//    UINT32 cAttrCount;
//    LPCSTR pszGuidStr;
//    std::string description;
//    WCHAR TempBuf[200];
//
//    if (pMediaType == NULL)
//    {
//        description = "<NULL>";
//        goto done;
//    }
//
//    hr = pMediaType->GetMajorType(&MajorType);
//    CHECKHR_GOTO(hr, done);
//
//    //pszGuidStr = STRING_FROM_GUID(MajorType);
//    pszGuidStr = GetGUIDNameConst(MajorType);
//    if (pszGuidStr != NULL)
//    {
//        description += pszGuidStr;
//        description += ": ";
//    }
//    else
//    {
//        description += "Other: ";
//    }
//
//    hr = pMediaType->GetCount(&cAttrCount);
//    CHECKHR_GOTO(hr, done);
//
//    for (UINT32 i = 0; i < cAttrCount; i++)
//    {
//        GUID guidId;
//        MF_ATTRIBUTE_TYPE attrType;
//
//        hr = pMediaType->GetItemByIndex(i, &guidId, NULL);
//        CHECKHR_GOTO(hr, done);
//
//        hr = pMediaType->GetItemType(guidId, &attrType);
//        CHECKHR_GOTO(hr, done);
//
//        pszGuidStr = GetGUIDNameConst(guidId);
//        if (pszGuidStr != NULL)
//        {
//            description += pszGuidStr;
//        }
//        else
//        {
//            LPOLESTR guidStr = NULL;
//            // GUID's won't have wide chars.
//            CHECKHR_GOTO(StringFromCLSID(guidId, &guidStr), done);
//            char str[128];
//            size_t wghat;
//            wcstombs_s(&wghat, str, guidStr, sizeof(str));
//            //wcstombs(str, guidStr, sizeof(str));
//            description += std::string(str);
//
//            CoTaskMemFree(guidStr);
//        }
//
//        description += "=";
//
//        switch (attrType)
//        {
//        case MF_ATTRIBUTE_UINT32:
//        {
//            UINT32 Val;
//            hr = pMediaType->GetUINT32(guidId, &Val);
//            CHECKHR_GOTO(hr, done);
//
//            description += std::to_string(Val);
//            break;
//        }
//        case MF_ATTRIBUTE_UINT64:
//        {
//            UINT64 Val;
//            hr = pMediaType->GetUINT64(guidId, &Val);
//            CHECKHR_GOTO(hr, done);
//
//            if (guidId == MF_MT_FRAME_SIZE)
//            {
//                description += "W:" + std::to_string(HI32(Val)) + " H: " + std::to_string(LO32(Val));
//            }
//            else if (guidId == MF_MT_FRAME_RATE)
//            {
//                // Frame rate is numerator/denominator.
//                description += std::to_string(HI32(Val)) + "/" + std::to_string(LO32(Val));
//            }
//            else if (guidId == MF_MT_PIXEL_ASPECT_RATIO)
//            {
//                description += std::to_string(HI32(Val)) + ":" + std::to_string(LO32(Val));
//            }
//            else
//            {
//                //tempStr.Format("%ld", Val);
//                description += std::to_string(Val);
//            }
//
//            //description += tempStr;
//
//            break;
//        }
//        case MF_ATTRIBUTE_DOUBLE:
//        {
//            DOUBLE Val;
//            hr = pMediaType->GetDouble(guidId, &Val);
//            CHECKHR_GOTO(hr, done);
//
//            //tempStr.Format("%f", Val);
//            description += std::to_string(Val);
//            break;
//        }
//        case MF_ATTRIBUTE_GUID:
//        {
//            GUID Val;
//            const char* pValStr;
//
//            hr = pMediaType->GetGUID(guidId, &Val);
//            CHECKHR_GOTO(hr, done);
//
//            pValStr = GetGUIDNameConst(Val);
//            if (pValStr != NULL)
//            {
//                description += pValStr;
//            }
//            else
//            {
//                LPOLESTR guidStr = NULL;
//                CHECKHR_GOTO(StringFromCLSID(Val, &guidStr), done);
//                char str[128];
//                size_t wghat;
//                wcstombs_s(&wghat, str, guidStr, sizeof(str));
//                //wcstombs(str, guidStr, sizeof(str));
//                description += std::string(str);
//
//                CoTaskMemFree(guidStr);
//            }
//
//            break;
//        }
//        case MF_ATTRIBUTE_STRING:
//        {
//            hr = pMediaType->GetString(guidId, TempBuf, sizeof(TempBuf) / sizeof(TempBuf[0]), NULL);
//            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
//            {
//                description += "<Too Long>";
//                break;
//            }
//            CHECKHR_GOTO(hr, done);
//            char str[200];
//            size_t wghat;
//            wcstombs_s(&wghat, str, TempBuf, sizeof(str));
//            //wcstombs(str, TempBuf, sizeof(str));
//            description += std::string(str);
//
//            break;
//        }
//        case MF_ATTRIBUTE_BLOB:
//        {
//            description += "<BLOB>";
//            break;
//        }
//        case MF_ATTRIBUTE_IUNKNOWN:
//        {
//            description += "<UNK>";
//            break;
//        }
//        }
//
//        description += ", ";
//    }
//
//done:
//
//    return description;
//}

struct VideoTexture::InternalData {

    IMFSourceResolver*  mySourceResolver = NULL;
    IUnknown*           myUnknownSource = NULL;
    IMFMediaSource*     myMediaFileSource = NULL;
    IMFAttributes*      myVideoReaderAttributes = NULL;
    IMFSourceReader*    mySourceReader = NULL;
    IMFMediaType*       myReaderOutputType = NULL, * myFirstOutputType = NULL;
    MF_OBJECT_TYPE      ObjectType = MF_OBJECT_INVALID;

    // Start processing frames.
    IMFSample*  myVideoSample = NULL;
    DWORD       myStreamIndex = 0, myFlags = 0;
    LONGLONG    myLongVideoTimeStamp = 0, myLongSampleDuration = 0;
    int         mySampleCount = 0;
    DWORD       mySampleFlags = 0;

    std::shared_ptr<Texture>    myTargetTexture = nullptr;
    uint32_t    myWidth = 0;
    uint32_t    myHeight = 0;
    float       myFps = 0.0f;

    float       myClockTime = 0.0f;
    LONGLONG    myVideoTime = 0;
    bool        myFinished = false;
    bool        myPaused = false;
    bool        myLooping = false;

    bool ReadOutputMediaFormat() {
        assert(mySourceReader);
        HRESULT hr;

        IMFMediaType* pNativeType = nullptr;
        CHECK_HRESULT(mySourceReader->GetNativeMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), static_cast<DWORD>(MF_SOURCE_READER_CURRENT_TYPE_INDEX), &pNativeType),
            "Error retrieving GetNativeMediaType.");

        SAFE_RELEASE(myFirstOutputType);
        CHECK_HRESULT(mySourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), &myFirstOutputType), "Error reading GetCurrentMediaType");

        // The sample uses the pNativeType here, but we want the resolution required to 'render' the texture as if the height is not x16 the conversion
        // YUV to RGB has problems
        CHECK_HRESULT(MFGetAttributeSize(myFirstOutputType, MF_MT_FRAME_SIZE, &myWidth, &myHeight), "Read input resolution");

        // Read fps
        uint64_t Val;
        hr = pNativeType->GetUINT64(MF_MT_FRAME_RATE, &Val);
        myFps = (float)HI32(Val) / (float)LO32(Val);

        SAFE_RELEASE(pNativeType);

        return true;
    }

public:

    ~InternalData() {
        if (myTargetTexture) 
        {
            myTargetTexture = nullptr;
        }
        SAFE_RELEASE(mySourceResolver);
        SAFE_RELEASE(myUnknownSource);
        SAFE_RELEASE(myMediaFileSource);
        SAFE_RELEASE(myVideoReaderAttributes);
        SAFE_RELEASE(mySourceReader);
        SAFE_RELEASE(myReaderOutputType);
        SAFE_RELEASE(myFirstOutputType);
    }

    bool Open(const char* aFilename) {
        myFinished = false;

        wchar_t wideFilename[256];
        size_t wghat;
        mbstowcs_s(&wghat, wideFilename, aFilename, sizeof(wideFilename) / sizeof(wchar_t));
        //mbstowcs(wideFilename, aFilename, sizeof(wideFilename) / sizeof(wchar_t));
        HRESULT hr = S_OK;

        // Need the color converter DSP for conversions between YUV, RGB etc.
        // Lots of examples use this explicit colour converter load. Seems
        // like most cases it gets loaded automatically.

       /* CHECK_HR(MFTRegisterLocalByCLSID(
          __uuidof(CColorConvertDMO),
          MFT_CATEGORY_VIDEO_PROCESSOR,
          L"",
          MFT_ENUM_FLAG_SYNCMFT,
          0,
          NULL,
          0,
          NULL),
          "Error registering colour converter DSP.");*/

          // Set up the reader for the file.
        CHECK_HRESULT(MFCreateSourceResolver(&mySourceResolver),
            "MFCreateSourceResolver failed.");

        CHECK_HRESULT(mySourceResolver->CreateObjectFromURL(
            wideFilename,		        // URL of the source.
            MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
            NULL,                       // Optional property store.
            &ObjectType,				        // Receives the created object type. 
            &myUnknownSource					          // Receives a pointer to the media source.
        ),
            "Failed to create media source resolver for file.");

        CHECK_HRESULT(myUnknownSource->QueryInterface(IID_PPV_ARGS(&myMediaFileSource)),
            "Failed to create media file source.");

        CHECK_HRESULT(MFCreateAttributes(&myVideoReaderAttributes, 2),
            "Failed to create attributes object for video reader.");

        CHECK_HRESULT(myVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID),
            "Failed to set dev source attribute type for reader config.");

        CHECK_HRESULT(myVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
            "Failed to set enable video processing attribute type for reader config.");

        CHECK_HRESULT(myVideoReaderAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV),
            "Failed to set media sub type on source reader output media type.");

        CHECK_HRESULT(MFCreateSourceReaderFromMediaSource(myMediaFileSource, myVideoReaderAttributes, &mySourceReader),
            "Error creating media source reader.");

        CHECK_HRESULT(MFCreateMediaType(&myReaderOutputType), "Failed to create source reader output media type.");
        CHECK_HRESULT(myReaderOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video), "Failed to set major type on source reader output media type.");
        CHECK_HRESULT(myReaderOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV), "Failed to set media sub type on source reader output media type.");

        CHECK_HRESULT(mySourceReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), NULL, myReaderOutputType),
            "Failed to set output media type on source reader.");

        if (!ReadOutputMediaFormat())
            return false;

        CHECK_HRESULT(mySourceReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS), FALSE), "Disable All Stream");
        CHECK_HRESULT(mySourceReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), TRUE), "Enable Video Stream");

        myClockTime = 0.0f;

        if (myTargetTexture)
        {
            myTargetTexture = nullptr;
        }

        // Just to ensure we have something as the first frame, and also ensure we have the real required size (height % 16 should be 0)
        Update(0.0f);

        return true;
    }

    void Update(float elapsed) {

        if (myPaused)
            return;

        if (myFinished)
            return;

        myClockTime += elapsed;

        LONGLONG uct = (LONGLONG)(myClockTime * 10000000); //vad fan är detta?
        if (uct < myVideoTime)
            return;

        assert(mySourceReader);
        myFlags = 0;

        HRESULT hr;
        hr = mySourceReader->ReadSample(
            static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM),
            0,                              // Flags.
            &myStreamIndex,                   // Receives the actual stream index. 
            &myFlags,                         // Receives status flags.
            &myLongVideoTimeStamp,              // Receives the time stamp.
            &myVideoSample                   // Receives the sample or NULL.
        );

        if (!SUCCEEDED(hr)) 
        {
            myFinished = true;
            return;
        }

        myVideoTime = myLongVideoTimeStamp;

        if (myFlags & MF_SOURCE_READERF_STREAMTICK)
        {
            //GELOG()
            //dbg("\tStream tick.\n");
        }
        if (myFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            //dbg("\tEnd of stream.\n");
            if (myLooping) {
                PROPVARIANT var = { 0 };
                var.vt = VT_I8;
                hr = mySourceReader->SetCurrentPosition(GUID_NULL, var);
                if (hr != S_OK) {/* dbg("Failed to set source reader position.");*/ }
                myClockTime = 0.0f;
            }
            else
                myFinished = true;
        }
        if (myFlags & MF_SOURCE_READERF_NEWSTREAM)
        {
            //dbg("\tNew stream.\n");
            myFinished = true;
        }
        if (myFlags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
        {
            //dbg("\tNative type changed.\n");
            myFinished = true;
        }
        if (myFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
           // dbg("\tCurrent type changed: from %dx%d %s (@ %f fps)\n", myWidth, myHeight, GetMediaTypeDescription(myFirstOutputType).c_str(), myFps);
            if (!ReadOutputMediaFormat()) {
                myFinished = true;
                return;
            }

        }

        if (myVideoSample) {
            hr = myVideoSample->SetSampleTime(myLongVideoTimeStamp);
            assert(SUCCEEDED(hr));

            hr = myVideoSample->GetSampleDuration(&myLongSampleDuration);
            assert(SUCCEEDED(hr));

            hr = myVideoSample->GetSampleFlags(&mySampleFlags);
            assert(SUCCEEDED(hr));

            // 
            IMFMediaBuffer* buf = NULL;
            DWORD bufLength;

            hr = myVideoSample->ConvertToContiguousBuffer(&buf);
            hr = buf->GetCurrentLength(&bufLength);

            byte* byteBuffer = NULL;
            DWORD buffMaxLen = 0, buffCurrLen = 0;
            hr = buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
            assert(SUCCEEDED(hr));

            // Some videos report one resolution and after the first frame change the height to the next multiple of 16 (using the event MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
            if (!myTargetTexture) 
            {
                
                int textureHeight = myHeight * 2;
                myTargetTexture = std::make_shared<Texture>(GraphicsEngine::Get().CreateTexture("VideoTexture", myWidth, textureHeight, RHITextureFormat::R8_UNORM, false, true, false, false, true));
            }

            //Clock clock;
            GraphicsEngine::Get().UpdateFromIYUV(byteBuffer, buffCurrLen, *myTargetTexture);
            //float elapsed = clock.elapsed();

            //dbg("Sample count %d, Sample flags %d, sample duration %I64d, sample time %I64d. CT:%I64d. Buffer: %ld/%ld. Elapsed:%f\n", mySampleCount, mySampleFlags, myLongSampleDuration, myLongVideoTimeStamp, uct, buffCurrLen, buffMaxLen, elapsed);

            hr = buf->Unlock();
            assert(SUCCEEDED(hr));

            SAFE_RELEASE(buf);
            mySampleCount++;

            SAFE_RELEASE(myVideoSample);
        }

    }

};








bool VideoTexture::CreateAPI()
{
	HRESULT hr = MFStartup(MF_VERSION);
	return SUCCEEDED(hr);
}

void VideoTexture::DestroyAPI()
{
	MFShutdown();
}

bool VideoTexture::Create(const char* aFilename)
{
	assert(!myInternalData);
	myInternalData = new InternalData();
	return myInternalData->Open(aFilename);
}

void VideoTexture::Destroy()
{
	if (myInternalData)
		delete myInternalData;
	myInternalData = nullptr;
}

bool VideoTexture::Update(float aDeltaTime)
{
	myInternalData->Update(aDeltaTime);
	return myInternalData->myFinished;
}

void VideoTexture::Pause()
{
	assert(myInternalData);
	myInternalData->myPaused = true;
}

void VideoTexture::Resume()
{
	assert(myInternalData);
	myInternalData->myPaused = false;
}

bool VideoTexture::HasFinished()
{
	assert(myInternalData);
	return myInternalData->myFinished;
}

std::shared_ptr<Texture> VideoTexture::GetTexture()
{
	assert(myInternalData);
	return myInternalData->myTargetTexture;
}

float VideoTexture::GetAspectRatio() const
{
	return (float)myInternalData->myWidth / (float)myInternalData->myHeight;
}

