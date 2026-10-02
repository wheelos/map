# Apollo Map 外部仓库迁移方案与计划

## 状态

本地抽取与 Apollo 消费端切换已实施；尚未完成容器内构建、回归测试和版本
发布。本方案记录当前边界、集成方式和剩余验收门槛。

## 目标与原则

将地图数据模型、HDMap 查询/解析能力及直接服务地图生产的工具，以独立
Bazel module 交付，并由 Apollo 主仓库通过 Bzlmod 依赖。采用“先在主仓库
建立可验证的 Bazel 目标，再抽取、再发布”的路径；避免一次性移动整个
`modules/map`。

边界原则：

1. 本仓库拥有可复用的地图库、格式解析适配器和地图生成工具。
2. Apollo 主仓库继续拥有产品组装、安装布局、运行配置、生产地图资产、
   routing topology builder 和 Planning 专用 `pnc_map`。
3. 外部仓库不依赖 Apollo 主仓库的 `//tools:install`、
   `//tools:cpplint` 等本地规则；发布目标由本仓库的 Bazel targets 定义。
4. 保持现有 C++ include 路径 `modules/map/...`，降低调用方迁移风险。
5. 先验证源代码和依赖图，再改变所有者；不得在抽取时顺便做无关 API
   重构。

## 建议范围

### 第一批迁入

- `modules/map/hdmap/**`：HDMap 核心、`hdmap_util`、OpenDRIVE adapter、
  XML parser，以及小型、可再分发的单元测试数据。
- `modules/map/tools` 中直接生成地图格式的工具：
  - `proto_map_generator`
  - `sim_map_generator`
  - `bin_map_generator`

首批工具与 HDMap / adapter 紧密相关，便于验证“解析/转换—地图查询”这一
完整边界。工具应作为 Bazel binary 提供，不将 Apollo 的安装目录结构带入
外部仓库。

### 第二批候选

- `map_tool`、`map_xysl`、`quaternion_euler`：在确认配置、common 工具库、
  Cyber 初始化和运行时资源路径均可由模块依赖显式提供后再迁入。
- 与地图采集服务相关的 `map_datachecker/{client,server,proto}`：依赖
  Cyber、gRPC、Apollo 消息和服务运行环境，作为独立子项目评估，不纳入
  首批发布。

### 暂留 Apollo 主仓库

- `modules/map/pnc_map`：它将 routing、VehicleState 和 HDMap 转成
  Planning 使用的 RouteSegments，属于规划侧适配层。
- `modules/map/data/**`、`modules/map/testdata/**` 中的产品/场景地图资产；
  需另定资产版本、授权和分发策略。
- `modules/routing/topo_creator/**` 及其调用流程；它由 routing 所有，即使
  调用外部 HDMap，也不因此归入 Map 仓库。
- Apollo 的安装聚合和产品运行时配置。

## Bazel 与 API 设计

### 仓库布局

在本仓库保留原有 source tree 前缀：

```text
MODULE.bazel
BUILD.bazel
modules/map/BUILD.bazel
modules/map/hdmap/...
modules/map/tools/...
```

保留 `modules/map/...` 目录前缀可以使现有 `#include "modules/map/..."` 不变。
对外标签初步采用以下形式，最终以实际 module name 和 target 拆分为准：

```text
@wheelos_map//modules/map/hdmap:hdmap
@wheelos_map//modules/map/hdmap:hdmap_util
@wheelos_map//modules/map/hdmap/adapter:opendrive_adapter
@wheelos_map//modules/map/tools:proto_map_generator
```

`wheelos_map` 是建议的 Bzlmod module name，实施前需检查私有 registry 中的
命名和版本策略；不要把仓库 Git URL、module name 与 apparent repository
name 混为一谈。

### 依赖边界

根据当前 BUILD 依赖，独立 module 至少需要显式声明并验证：

- `wheelos_msgs`：地图消息及 navigation 消息；
- `wheelos_common`：配置、math、status、util、投影等共用能力；
- `wheelos_core`，在本模块内按需映射为 `@core`；
- 当前 BUILD 直接使用的 Abseil、glog、gflags、PROJ、tinyxml2 等 Bazel
  modules。

Bzlmod 的根模块依赖不会自动成为外部 module 的直接依赖。所有 BUILD 中
直接引用的 apparent repository 都要由本模块的 `MODULE.bazel` 声明，并验证
版本图与 Apollo 根模块能够解析到兼容版本。避免通过复制 Apollo 根模块的
整个依赖清单来“解决”缺依赖。

对 `wheelos_core` 使用实际 API 对应的最小 Cyber targets，而不是聚合的
`@core//cyber`：

- `@core//cyber/common:file`：文件和 protobuf 读写；
- `@core//cyber/common:log`：Cyber 日志宏；
- `@core//cyber/common:macros`：公共头文件中的 Cyber 宏。

从外部 BUILD 删除或替换 Apollo 私有的 install/lint 宏依赖。测试数据采用
小型、可审查、可再分发的 fixture；生产地图、运行配置和安装产物不得混入
库 target。

## 当前实施状态

- 本仓库已有 `MODULE.bazel`（`wheelos_map` 0.1.0）和 HDMap、adapter、
  XML parser、三个地图生成器的 Bazel targets。
- Apollo 主仓库已移除 `modules/map/hdmap` 和三个已抽取生成器的源文件；
  相关消费者改为 `@wheelos_map` targets，C++ include 路径保持不变。
- Apollo 根模块通过 root-only `local_path_override` 消费本地 checkout。
  开发容器将 sibling `map` checkout 只读挂载到 `/apollo/data/map`；
  `MAP_REPO_ROOT` 可用于指定其他 host checkout 位置。
- Apollo 的 map 安装聚合留在消费端；三个生成器通过本仓库直接提供，
  Apollo 保留兼容 alias 和 `gen_all_map.sh` 使用入口。
- Map module 以及仍留在 Apollo 的 `pnc_map` 和地图工具使用细粒度 Cyber
  `file` / `log` / `macros` targets；未抽取的 map_datachecker 服务不在本次范围。
- 尚待完成：实际 Bzlmod 解析与容器构建、HDMap tests、routing/planning/
  DreamView 消费者回归、安装路径复核和可发布版本准备。

## 分阶段实施计划

### 阶段 0：冻结边界与建立清单

- 记录源模块基线、目标 BUILD labels、头文件 include 路径、外部依赖和测试
  数据来源。
- 逐项检查 HDMap、adapter、三种地图生成工具的 C++ include、运行时配置、
  Cyber 初始化和资源查找假设。
- 明确第一批不包含 `pnc_map`、地图资产、topo_creator、map_datachecker。
- 产出：迁移清单和依赖图；有未确认授权或运行时依赖的项先标为阻塞，不搬迁。

### 阶段 1：在 Apollo 主仓库形成可发布边界

- 将 HDMap、adapter 和首批工具的 BUILD 目标整理为不依赖 install 聚合宏的
  普通 library/binary/test targets。
- 为外部消费者定义稳定的公共 targets；收紧非 API targets 的 visibility。
- 以 Apollo 根仓库现有构建验证原 targets 和新边界 target 行为一致。
- 产出：主仓库内可独立构建、可测试的 map release surface。

### 阶段 2：准备独立仓库与最小模块闭包

- 在 `/home/wfh/01code/map` 建立 `MODULE.bazel`、根 `BUILD.bazel` 和对应
  packages，按既定布局接收阶段 1 的文件。
- 导入必需 BUILD、源文件、license/notice 和最小单元测试 fixture；不导入
  Apollo 安装脚本、无关工具或产品数据。
- 声明模块直接依赖，验证 standalone module graph；处理公开头文件的传递依赖
  和 generated proto 的可见性。
- 产出：不依赖 Apollo 主仓库本地标签即可分析、构建和测试的独立仓库。

### 阶段 3：Apollo 消费端切换

- 在 Apollo 根 `MODULE.bazel` 加入版本化 `bazel_dep`；本地联调时仅在消费端
  使用 root-owned `local_path_override` 指向 `/home/wfh/01code/map`。
- 将所有使用方 BUILD 中的 `//modules/map/hdmap...` 标签迁为外部模块标签，
  覆盖 planning、routing、prediction、perception、DreamView、storytelling
  和地图工具调用方。
- 保持 C++ include 路径不变；检查根仓库安装目标仍能正确组装 Apollo 产品。
- 产出：使用本地 override 的 Apollo 主仓库可完整构建相关消费者。

### 阶段 4：回归、版本化与发布

- 独立运行 HDMap、adapter 和首批工具的测试；验证地图格式输出可被现有 Apollo
  routing / planning 消费。
- 在 Apollo 消费端验证核心依赖者，重点覆盖 routing topology 创建、Planning
  map 查询、Prediction、DreamView 地图服务。
- 记录兼容版本、发布 notes、变更影响和 registry 元数据，再发布首个版本。
- 产出：可复现的版本化 module 与主仓库锁定版本。

### 阶段 5：评估其余工具

- 按工具分别核实通用性、产品消息依赖、Cyber runtime、配置/数据定位、部署
  责任和消费者数量。
- 只有当目标能独立分析、测试、运行且有清晰 owner 后，才迁移第二批工具。
- 对 map_datachecker 单独形成服务模块边界评审；不以“位于 tools 目录”为迁出
  依据。

## 验收条件

1. `wheelos_map` 可在不加载 Apollo 主仓库 BUILD 文件的情况下解析依赖、构建
   和运行其承诺支持的 tests/binaries。
2. HDMap 和 adapter 对外 labels、公共头文件及其依赖明确；不泄漏 install
   macro、产品路径或未声明的 transitive dependency。
3. 首批工具的输出格式、地图加载行为与迁移前兼容；地图生成所需配置和运行时
   资源均有显式输入契约。
4. Apollo 消费端相关模块通过构建和针对性回归；依赖方向保持
   `Apollo consumers -> wheelos_map`，不出现 map 反向依赖 planning/routing。
5. 主仓库继续单独拥有产品地图资产、产品打包规则和 routing topology 工作流。
6. 使用者可通过已发布版本复现构建；本地 override 仅用于协同开发，不作为
   生产依赖方式。

## 风险与待决项

- **消息/公共 API 兼容：** `map_cc_proto` 和 navigation 消息属于外部协议依赖；
  需锁定支持范围，避免 module 接受无法兼容的 wheelos_msgs 版本。
- **运行时隐式配置：** HDMap 工具使用配置 gflags、Cyber 和 map path 约定；
  需明确初始化责任和资源路径，不将“能链接”误当成“可运行”。
- **Bazel 依赖图：** 外部 module 需自行声明 direct deps；根模块现有声明不能
  掩盖 module 内的缺失依赖。
- **产品安装：** 原 `modules/map/BUILD` 聚合 `install` targets；抽取后 Apollo
  仍需由消费端负责安装路径和 bundle 组合。
- **测试/资产授权：** 当前 test-data 和 data 内容必须逐个审查，不能默认都能
  复制到独立仓库或公开分发。
- **消费者迁移面：** HDMap 被多个子系统直接依赖；必须以全量 BUILD 引用扫描
  和构建覆盖为迁移门槛。

## 主要源代码依据

- `apollo-lite/modules/map/hdmap/BUILD`
- `apollo-lite/modules/map/hdmap/adapter/BUILD`
- `apollo-lite/modules/map/hdmap/adapter/xml_parser/BUILD`
- `apollo-lite/modules/map/tools/BUILD`
- `apollo-lite/modules/map/tools/map_datachecker/{client,server}/BUILD`
- `apollo-lite/modules/map/BUILD`
- `apollo-lite/MODULE.bazel`
- `apollo-lite/wheelos-service/context/framework/build/modular-release-repository-strategy.md`
