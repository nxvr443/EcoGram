import os, re

def patch_file(path, old, new):
    if not os.path.exists(path):
        print(f"[SKIP] File not found: {path}")
        return False
    with open(path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    if old not in content:
        print(f"[WARN] Target not found in: {path}")
        return False
    content = content.replace(old, new, 1)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"[OK] Patched: {path}")
    return True

base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

# 1. Patch tgcalls/MediaManager.cpp
media_manager = os.path.join(base_dir, 'ThirdParty', 'tgcalls', 'tgcalls', 'MediaManager.cpp')
patch_file(
    media_manager,
    '"WebRTC-Audio-Allocation/min:32kbps,max:32kbps/"',
    '"WebRTC-Audio-Allocation/min:32kbps,max:64kbps/"\n\t\t"WebRTC-Dscp/Enabled/"'
)

old_opus_params = """        const uint8_t opusMinBitrateKbps = 6;
        const uint8_t opusMaxBitrateKbps = 32;
        const uint8_t opusStartBitrateKbps = 8;
        const uint8_t opusPTimeMs = 120;"""

new_opus_params = """        const uint8_t opusMinBitrateKbps = 24; // EcoGram Turbo VoIP: HD Voice floor
        const uint8_t opusMaxBitrateKbps = 64; // EcoGram Turbo VoIP: 64 kbps Studio Opus ceiling
        const uint8_t opusStartBitrateKbps = 32; // EcoGram Turbo VoIP: Start crystal clear immediately
        const uint8_t opusPTimeMs = 20; // EcoGram Turbo VoIP: 20ms frames cut latency and eliminate packet drops over VPN"""

patch_file(media_manager, old_opus_params, new_opus_params)

# 2. Patch tgcalls/EncryptedConnection.cpp (Safe MTU for VPN tunnels)
encrypted_conn = os.path.join(base_dir, 'ThirdParty', 'tgcalls', 'tgcalls', 'EncryptedConnection.cpp')
patch_file(
    encrypted_conn,
    'constexpr auto kMaxFullPacketSize = 1500; // IP_PACKET_SIZE from webrtc.',
    'constexpr auto kMaxFullPacketSize = 1280; // EcoGram Safe MTU (1280B) prevents VPN/TUN fragmentation'
)

# 3. Patch tgcalls/group/GroupInstanceCustomImpl.cpp
group_impl = os.path.join(base_dir, 'ThirdParty', 'tgcalls', 'tgcalls', 'group', 'GroupInstanceCustomImpl.cpp')
patch_file(
    group_impl,
    'const uint8_t opusPTimeMs = 120;',
    'const uint8_t opusPTimeMs = 20; // EcoGram Turbo VoIP 20ms frame'
)

print("VoIP Turbo patching complete!")
