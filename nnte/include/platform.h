/***************************************************
#filename      : platform.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-31 16:50:50
#last modified : 2023-06-05 21:00:56
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_PLATFORM_H_
#define NNTE_INCLUDE_PLATFORM_H_

#include <cstdint>
#include <algorithm>
#include <map>

#include "./nnte_utils.h"

namespace nnte {

typedef enum class NpuPlatform {
  NPU_V1 = 0,
  NPU_V2,
  NPU_V3
} NpuPlatform;

NpuPlatform inline GetPlatform() {
    return NpuPlatform::NPU_V2;
}

typedef struct Mac {
  int h;
  int w;
} Mac;

typedef struct Vme {
  int w;
} Vme;


typedef struct Npc {
  Mac mac_;
  Vme vme_;
} Npc;

std::map<NpuPlatform, Npc> npcs = {
  {
    NpuPlatform::NPU_V1, {
      .mac_ = {.h = 32, .w = 64},
      .vme_ = {.w = 64}
    },
  },
  {
    NpuPlatform::NPU_V2, {
      .mac_ = {.h = 64, .w = 64},
      .vme_ = {.w = 64}
    }
  }
};


uint64_t MmePerfEval(int lh, int lw, int rw) {
  Npc npc = npcs[GetPlatform()];
  int mac_h = npc.mac_.h;
  int mac_w = npc.mac_.w;
  uint64_t mme_cycles = (2 + ceil_2x(lh, mac_h) / mac_h * ceil_2x(lw, mac_w) / mac_w * ceil_2x(rw, mac_w) / mac_w) * mac_h;
  return mme_cycles;
}

uint64_t VmePerfEval(int lh, int lw, int rh, int rw) {
    int m = std::max(lh, rh);
    int n = std::max(lw, rw);
    int t = 3;  // match for hardware
    int k = 1;
    Npc npc = npcs[GetPlatform()];
    int vme = npc.vme_.w;
    uint64_t vme_cycles = 10 + m * (n + vme - 1) / vme * t * k;
    return vme_cycles;
}

typedef enum class TransferChannel {
  HOST_GDRAM = 0,
  GDRAM_GDRAM,
  GDRAM_LLB,
  GDRAM_L1,
  LLB_LLB,
  LLB_L1_B,
  LLB_L1,
  L1_L1
} TransferChannel;

#define NPU_V1_PCIE (32)
#define NPU_V1_GDRAM (32)
#define NPU_V1_LLB (128)
#define NPU_V1_L1 NPU_V1_LLB

#define NPU_V2_PCIE (32)
#define NPU_V2_GDRAM (48)
#define NPU_V2_LLB  NPU_V1_LLB
#define NPU_V2_L1 NPU_V1_LLB

#define NPU_V3_PCIE (32)
#define NPU_V3_GDRAM (48)
#define NPU_V3_LINK (400/8)
#define NPU_V3_LLB NPU_V1_LLB
#define NPU_V3_L1 NPU_V1_LLB

std::map<NpuPlatform, std::map<TransferChannel, int>> transfer_bandwidth = {
  {
    NpuPlatform::NPU_V1, {
      {TransferChannel::HOST_GDRAM, NPU_V1_PCIE},
      {TransferChannel::GDRAM_GDRAM, NPU_V1_PCIE},
      {TransferChannel::GDRAM_LLB, NPU_V1_GDRAM},
      {TransferChannel::GDRAM_L1, NPU_V1_GDRAM},
      {TransferChannel::LLB_LLB, NPU_V2_LLB},
      {TransferChannel::LLB_L1_B, NPU_V2_LLB},
      {TransferChannel::LLB_L1, NPU_V2_LLB},
      {TransferChannel::L1_L1, NPU_V2_L1}
    }
  },
  {
    NpuPlatform::NPU_V2,{
      {TransferChannel::HOST_GDRAM, NPU_V2_PCIE},
      {TransferChannel::GDRAM_GDRAM, NPU_V2_PCIE},
      {TransferChannel::GDRAM_LLB, NPU_V2_GDRAM},
      {TransferChannel::GDRAM_L1, NPU_V2_GDRAM},
      {TransferChannel::LLB_LLB, NPU_V2_LLB},
      {TransferChannel::LLB_L1_B, NPU_V2_LLB},
      {TransferChannel::LLB_L1, NPU_V2_LLB},
      {TransferChannel::L1_L1, NPU_V2_L1}
    }
  },
  {
    NpuPlatform::NPU_V3,{
      {TransferChannel::HOST_GDRAM, NPU_V3_PCIE},
      {TransferChannel::GDRAM_GDRAM, NPU_V3_PCIE},
      {TransferChannel::GDRAM_LLB, NPU_V3_GDRAM},
      {TransferChannel::GDRAM_L1, NPU_V3_GDRAM},
      {TransferChannel::LLB_LLB, NPU_V2_LLB},
      {TransferChannel::LLB_L1_B, NPU_V2_LLB},
      {TransferChannel::LLB_L1, NPU_V2_LLB},
      {TransferChannel::L1_L1, NPU_V2_L1}
    }
  }
};

uint64_t DmaPerfEval(int lh, int lw, int bw) {
  uint64_t cycles_dma = (uint64_t)lh * lw * bw / transfer_bandwidth[GetPlatform()][TransferChannel::GDRAM_L1];
  return cycles_dma;
}

uint64_t TransDataPerfEval(int lh, int lw, int bw, const TransferChannel & channel) {
  uint64_t cycles_transfer = (uint64_t)lh * lw * bw / transfer_bandwidth[GetPlatform()][channel];
  return cycles_transfer;
}

}  // namespace nnte

#endif // NNTE_INCLUDE_PLATFORM_H_

