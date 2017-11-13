//
//  main.c
//  AQPlayer
//
//  Created by Kazuya Shida on 2017/11/12.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <AudioToolbox/AudioToolbox.h>

#define kNumOfPlaybackBuffer 3

#pragma mark - Audio Queue Info

typedef struct AQPlayerContInfo {
    AudioFileID playbackFile; // file object
    SInt64 packetPosition;    // current packet postition
    UInt32 numPacketsToRead;
    AudioStreamPacketDescription *packetDescription; // packet
    Boolean isRunning; // is playing if true, otherwise false
} AQPlayerContInfo;

#pragma mark - CalculateBufferSize

void CalculateBufferSize(AudioFileID inAudioFile, AudioStreamBasicDescription inAudioStreamBasicDesc, Float64 inSeconds, UInt32 *outBufferSize, UInt32 *outNumPacketsToRead)
{
    UInt32 maxPacketSize;
    UInt32 propertySize = sizeof(maxPacketSize);
    AudioFileGetProperty(inAudioFile, kAudioFilePropertyPacketSizeUpperBound, &propertySize, &maxPacketSize);
    static const int maxBufferSize = 0x10000;
    static const int minBufferSize = 0x4000;
    if (inAudioStreamBasicDesc.mFramesPerPacket) {
        // CBR, VBR
        Float64 numPacketsForTime =
            inAudioStreamBasicDesc.mSampleRate / inAudioStreamBasicDesc.mFramesPerPacket * inSeconds;
        *outBufferSize = numPacketsForTime * maxPacketSize;
    } else {
        // VFR
        *outBufferSize = maxBufferSize > maxPacketSize ? maxBufferSize : maxPacketSize;
    }
    
    if ((*outBufferSize > maxBufferSize) && (*outBufferSize > maxPacketSize)) {
        *outBufferSize = maxBufferSize;
    } else {
        if (*outBufferSize < minBufferSize) {
            *outBufferSize = minBufferSize;
        }
    }
    *outNumPacketsToRead = *outBufferSize / maxPacketSize;
}

#pragma mark - CopyDecodeMagicCookieToQueue

void CopyDecodeMagicCookieToQueue(AudioFileID file, AudioQueueRef queue)
{
    UInt32 propertySize;
    OSStatus result = AudioFileGetPropertyInfo(file, kAudioFilePropertyMagicCookieData, &propertySize, NULL);
    if (result == noErr && propertySize > 0) {
        UInt8 *magicCookie = (UInt8 *)malloc(sizeof(UInt8) * propertySize);
        AudioFileGetProperty(file, kAudioFilePropertyMagicCookieData, &propertySize, magicCookie);
        AudioQueueSetProperty(queue, kAudioQueueProperty_MagicCookie, magicCookie, propertySize);
        free(magicCookie);
    }
}

#pragma mark -

static void MyAudioQueueOutputCallback(void *inSoundData, AudioQueueRef inAQ, AudioQueueBufferRef enQueueAQBuffer)
{
    AQPlayerContInfo *aqp = (AQPlayerContInfo *)inSoundData;
    if (!aqp->isRunning) {
        return;
    }
    
    UInt32 numBytes;
    UInt32 nPackets = aqp->numPacketsToRead;
    AudioFileReadPackets(aqp->playbackFile, FALSE, &numBytes, aqp->packetDescription, aqp->packetPosition, &nPackets, enQueueAQBuffer->mAudioData);
//    AudioFileReadPacketData(aqp->playbackFile, FALSE, &numBytes, aqp->packetDescription, aqp->packetPosition, &nPackets, enQueueAQBuffer->mAudioData);
    
    if (nPackets > 0) {
        enQueueAQBuffer->mAudioDataByteSize = numBytes;
        AudioQueueEnqueueBuffer(inAQ, enQueueAQBuffer, (aqp->packetDescription ? nPackets : 0), aqp->packetDescription);
        aqp->packetPosition += nPackets;
    } else {
        AudioQueueStop(inAQ, FALSE);
        aqp->isRunning = FALSE;
    }
}

#pragma mark - main

int main(int argc, const char * argv[])
{
    AQPlayerContInfo player = {0};
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    
    const char *filename = argv[1];
    CFURLRef audioFileURL =
        CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)filename, strlen(filename), false);
    AudioFileOpenURL(audioFileURL, 0x1, 0, &player.playbackFile);
    CFRelease(audioFileURL);
    
    AudioStreamBasicDescription dataFormat;
    UInt32 propSize = sizeof(dataFormat);
    OSStatus result = AudioFileGetProperty(player.playbackFile, kAudioFilePropertyDataFormat, &propSize, &dataFormat);
    
    if (result != noErr) {
        fprintf(stderr, "Error: %d\n", result);
        exit(1);
    }
    
    AudioQueueRef queue;
    AudioQueueNewOutput(&dataFormat, MyAudioQueueOutputCallback, &player, NULL, NULL, 0, &queue);
    
    UInt32 bufferBytesSize;
    CalculateBufferSize(player.playbackFile, dataFormat, 0.5, &bufferBytesSize, &player.numPacketsToRead);
    
    bool isFormatVBR = (dataFormat.mBytesPerPacket == 0) || (dataFormat.mFramesPerPacket == 0);
    if (isFormatVBR) {
        player.packetDescription = (AudioStreamPacketDescription *)malloc(sizeof(AudioStreamPacketDescription) * player.numPacketsToRead);
    } else {
        player.packetDescription = NULL;
    }
    
    CopyDecodeMagicCookieToQueue(player.playbackFile, queue);
    
    AudioQueueBufferRef buffers[kNumOfPlaybackBuffer];
    player.isRunning = TRUE;
    player.packetPosition = 0;
    for (int i = 0; i < kNumOfPlaybackBuffer; ++i) {
        AudioQueueAllocateBuffer(queue, bufferBytesSize, &buffers[i]);
        MyAudioQueueOutputCallback(&player, queue, buffers[i]);
        if (!player.isRunning) {
            break; // End
        }
    }
    
    AudioQueueStart(queue, NULL);
    printf("Play start\n");
    printf("%s\n", filename);
    
    do {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.25, FALSE);
    } while (player.isRunning);
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 2, FALSE);
    printf("Play end\n");
    
    AudioQueueDispose(queue, TRUE);
    AudioFileClose(player.playbackFile);
    return 0;
}
