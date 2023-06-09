/***************************************************
#filename      : platform.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-31 16:50:50
#last modified : 2023-06-08 19:29:15
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

typedef enum class Topo {
  RING_ROW = 0,
  RING_COLUMN,
  RING_MESH,
} Topo;


NpuPlatform platform = NpuPlatform::NPU_V2;
Topo topo = Topo::RING_MESH;
int chip_num = 1;
int cluster_num = 4;
int core_num = 8;


NpuPlatform inline GetPlatform() {
    return platform;
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
  },
  {
    NpuPlatform::NPU_V3, {
      .mac_ = {.h = 64, .w = 64},
      .vme_ = {.w = 64}
    }
  },
};

std::map<std::string, NpuPlatform> str_2_npu = {
  {"V1", NpuPlatform::NPU_V1},
  {"V2", NpuPlatform::NPU_V2},
  {"V3", NpuPlatform::NPU_V3},
};

std::map<std::string, Topo> str_2_topo = {
  {"ROW", Topo::RING_ROW},
  {"MESH", Topo::RING_MESH},
  {"COLUMN", Topo::RING_COLUMN},
};

void InitNnte(std::string version, int cpn, std::string tp) {
  std::transform(version.begin(), version.end(), version.begin(), ::toupper);
  std::transform(tp.begin(), tp.end(), tp.begin(), ::toupper);
  platform = str_2_npu[version];
  chip_num = cpn;
  topo = str_2_topo[tp];
}

int GetCoreNum() {
  return core_num * cluster_num * chip_num;
}

int GetClusterNum() {
  return cluster_num * chip_num;
}

int GetChipNum() {
  return chip_num;
}

std::map<Topo, int> ports = {
  {Topo::RING_ROW, 4},
  {Topo::RING_COLUMN, 4},
  {Topo::RING_MESH, 2},
};

Topo GetTopo() {
  return topo;
}

int GetLinkPortNum() {
  return ports[GetTopo()];
}


uint64_t MmePerfEval(int lh, int lw, int rw) {
  Npc npc = npcs[GetPlatform()];
  int mac_h = npc.mac_.h;
  int mac_w = npc.mac_.w;
  uint64_t mme_cycles = (2 + ceil_2x(lh, mac_h) / mac_h * ceil_2x(lw, mac_w) / mac_w * ceil_2x(rw, mac_w) / mac_w) * mac_h;
  return mme_cycles / GetCoreNum();
}

uint64_t VmePerfEval(int lh, int lw, int rh, int rw) {
    int m = std::max(lh, rh);
    int n = std::max(lw, rw);
    int t = 3;  // match for hardware
    int k = 1;
    Npc npc = npcs[GetPlatform()];
    int vme = npc.vme_.w;
    uint64_t vme_cycles = 10 + m * (n + vme - 1) / vme * t * k;
    return vme_cycles / GetCoreNum();
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
#define NPU_V3_GDRAM (225) // 225 = 900 / 4
#define NPU_V3_LINK (400/8)
#define NPU_V3_LLB (NPU_V3_GDRAM*2)
#define NPU_V3_L1 NPU_V3_LLB

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
      {TransferChannel::LLB_L1_B, NPU_V2_LLB * 8},
      {TransferChannel::LLB_L1, NPU_V2_LLB},
      {TransferChannel::L1_L1, NPU_V2_L1}
    }
  },
  {
    NpuPlatform::NPU_V3,{
      {TransferChannel::HOST_GDRAM, NPU_V3_PCIE},
      {TransferChannel::GDRAM_GDRAM, NPU_V3_LINK},
      {TransferChannel::GDRAM_LLB, NPU_V3_GDRAM},
      {TransferChannel::GDRAM_L1, NPU_V3_GDRAM},
      {TransferChannel::LLB_LLB, NPU_V3_LLB},
      {TransferChannel::LLB_L1_B, NPU_V3_LLB * 8} ,
      {TransferChannel::LLB_L1, NPU_V3_LLB},
      {TransferChannel::L1_L1, NPU_V3_L1}
    }
  }
};

uint64_t DmaPerfEval(int lh, int lw, int bw) {
  uint64_t cycles_dma = (uint64_t)lh * lw * bw / transfer_bandwidth[GetPlatform()][TransferChannel::GDRAM_L1];
  return cycles_dma;
}

uint64_t TransDataPerfEval(int lh, int lw, int bw, const TransferChannel & channel) {
  uint64_t xfer_bw = transfer_bandwidth[GetPlatform()][channel];
  double data_gb = (double) lh * lw * bw / 1024 / 1024 /1024;
  uint64_t cycles_transfer = data_gb / xfer_bw * 1000 * 1000 * 1000;
  if (channel == TransferChannel::GDRAM_L1 || channel == TransferChannel::GDRAM_LLB
      || channel == TransferChannel::LLB_LLB || channel == TransferChannel::LLB_L1_B
      || channel == TransferChannel::LLB_L1
      ) {
    cycles_transfer = cycles_transfer / GetClusterNum();
  }
  if (channel == TransferChannel::GDRAM_GDRAM) {
    // cycles_transfer /= GetLinkPortNum();
  }
  return cycles_transfer;
}

}  // namespace nnte

#endif // NNTE_INCLUDE_PLATFORM_H_

