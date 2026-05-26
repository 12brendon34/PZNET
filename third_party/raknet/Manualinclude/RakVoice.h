//
// Created by Brendon on 5/26/2026.
//



#ifndef __RAK_VOICE_H
#define __RAK_VOICE_H

namespace RakNet {

    class __declspec(dllimport) RakNet::RakVoice {
        public:
        RakVoice();
        virtual ~RakVoice();

        void CloseAllChannels();
        void CloseVoiceChannel(RakNet::RakNetGUID);
        void Deinit();
        int GetBufferSizeBytes();
        int GetBuffering();
        bool GetChannelStatistics(RakNet::RakNetGUID,__int64 *);
        int GetEncoderComplexity();
        bool GetIs3D();
        float GetMaxDistance();
        float GetMinDistance();
        RakNet::RakPeerInterface * GetRakPeerInterface() const;
        int GetSampleRate();
        int GetSendFramePeriod();
        bool GetServerVOIPEnable();
        void Init(unsigned int); //doesn't take buffersize aparently.
        void InitServer(bool,unsigned short,int,int,int,float,float,bool);
        bool IsInitialized();
        bool IsLoopbackMode() const;
        int OPUS_test(int,int,int);
        virtual void OnClosedConnection(RakNet::SystemAddress const &,RakNet::RakNetGUID,RakNet::PI2_LostConnectionReason);
        void ReceiveFrame(void *outputBuffer);
        void RequestVoiceChannel(RakNet::RakNetGUID);
        void ResetChannel(RakNet::VoiceChannel *);
        bool SendFrame(RakNetGUID recipient, void *inputBuffer);
        void SetChannelsRouting(RakNet::RakNetGUID,bool,int *,short);
        void SetEncoderComplexity(int);
        void SetLoopbackMode(bool);
        void SetVoiceBan(int,bool);


        // --------------------------------------------------------------------------------------------
        // Message handling functions
        // --------------------------------------------------------------------------------------------
        virtual void OnShutdown(void);
        virtual void Update(void);
        virtual PluginReceiveResult OnReceive(Packet *packet);
        virtual void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
        protected:
        void OnOpenChannelRequest(Packet *packet);
        void OnOpenChannelReply(Packet *packet);
        virtual void OnVoiceData(Packet *packet);
        void OpenChannel(Packet *packet);
        void FreeChannelMemory(RakNetGUID recipient);
        void FreeChannelMemory(unsigned index, bool removeIndex);
        void WriteOutputToChannel(VoiceChannel *channel, char *dataToWrite);
        void SetEncoderParameter(void* enc_state, int vartype, int val);
        void SetPreprocessorParameter(void* pre_state, int vartype, int val);

        DataStructures::OrderedList<RakNetGUID, VoiceChannel*, VoiceChannelComp> voiceChannels;
        int32_t sampleRate;
        unsigned bufferSizeBytes;
        float *bufferedOutput;
        unsigned bufferedOutputCount;
        bool zeroBufferedOutput;
        int defaultEncoderComplexity;
        bool defaultVADState;
        bool defaultDENOISEState;
        bool defaultVBRState;
        bool loopbackMode;

    };

} // namespace RakNet

#endif
