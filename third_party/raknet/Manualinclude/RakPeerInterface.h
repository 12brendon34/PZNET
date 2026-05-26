

#ifndef __RAK_PEER_INTERFACE_H
#define __RAK_PEER_INTERFACE_H

#include "Export.h"

namespace RakNet {
    // Forward declarations
    class BitStream;
    class PluginInterface2;
    struct RPCMap;
    struct RakNetStatistics;
    struct RakNetBandwidth;
    class RouterInterface;
    class NetworkIDManager;

    class RAK_DLL_EXPORT RakPeerInterface {
    public:
        // GetInstance() and DestroyInstance(instance*)
        STATIC_FACTORY_DECLARATIONS(RakPeerInterface)
        RakPeerInterface(RakPeerInterface const &);
        RakPeerInterface();
        ~RakPeerInterface();
        RakPeerInterface& operator=(RakPeerInterface const &);
        //static void DestroyInstance(RakPeerInterface *);
	    static uint64_t Get64BitUniqueRandomNumber(void);
        //static RakPeerInterface* GetInstance();
    };
} // namespace RakNet

#endif
