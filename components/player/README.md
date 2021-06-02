# Player

## 1、介绍

**Player** 是 RT-Thread 开发的适合 MCU 的轻型流媒体音频框架，资源占用小，响应快。

RT-Thread 提供的 Player 组件具有以下优势：

- 本地播放：支持播放本地文件系统中的音频文件
- 网络播放: 支持播放网络数据流歌曲，支持 `http`、`https`、`HLS`、`PLS`、`TTS` 流式音频播放
- 支持多种音频格式：默认支持 `pcm`、`wav` 音频格式
- 支持多种容器格式：默认支持 `MPEG-TS` 音频容器
- 支持外置注册解码器，支持 `mp3`、`aac`，`m4a`、`flac`、`amr`、`ogg`、`wma`、`flac` 格式音频外置注册
- 支持自定义加密音频格式
- 支持歌单播放功能：支持 `添加`、`删除`、`播放`、`暂停`、`上一曲`、`下一曲`、`切换歌单` 等多种歌单播放功能

更多详细介绍，参考 [Player 介绍](docs/introduction.md) 文档。

### 1.1 目录结构

> player packages 源码 的目录结构

| 名称 | 说明 |
| ---- | ---- |
| docs | Player 组件相关文档 |
| example | Player 组件使用例程 |
| player  | Player 组件源码 |
| .gitignore | git 忽略文件, 使用者无需关心 |
| LICENSE | 许可声明 |
| README.md | 简单介绍 |
| SConscript | Player 组件 scons 脚本，用于组织 player 源码编译方式 |

### 1.2 许可证

**Player** 软件包遵循 GPL2+ 商业双许可。该软件包可以根据 GUN 标准使用通用公共许可证，详见 LICENSE 文件。如果用于商业应用，可以通过电子邮箱 <business@rt-thread.com > 与我们联系获取商业许可。

### 1.3 依赖

- RT-Thread 3.0+
- [LwIP](https://github.com/RT-Thread/rt-thread/tree/master/components/net/lwip-2.0.2)
- [cJson](https://github.com/RT-Thread-packages/cJSON)
- [WebClient](https://github.com/RT-Thread-packages/webclient)
    - player_v1.2.4 之前的版本依赖 WebClient_v1.0.0
    - player_v1.2.4 之后版本(包括)依赖于 WebClient_v2.x.x
- [DFS](https://github.com/RT-Thread/rt-thread/tree/master/components/dfs)
- [Finsh](https://github.com/RT-Thread/rt-thread/tree/master/components/finsh)
- [Codec](None)

## 2、如何配置 Player

**Player** 通常是以基础组件包的形式自动被其它组件依赖，但开发者也可以使用 RT-Thread 的包管理器中选择使用它，具体路径如下：

```
Privated Packages of RealThread
    [*] Player: The RT-Thread lightweight streaming audio playback package
          version (v1.2.7_alpha(Testing))  --->
          examples  --->
              [*] Using player msh command
              [*] Using listplayer msh command
              [*] Using codec dump msh command
              [*] Using tts example and msh test command
          Build (Release)
              *** Release version without debug log ***
              (X) Debug
              ( ) Release
   (sound0) Device Name
       (80) Startup default volume
          Audio codec configuration  --->
              *** The following codecs and formats are built-in implementations of player ***
              [ ] Enable pcm codec support
              [ ] Enable wav codec support
              [ ] Enable MPEG-TS formats support
          Net related configuration  --->
              [*] Enable net stream
              (128) net stream buffer size(KB)
              [ ] Enable TTS stream
              [ ] Enable HLS stream
              [ ]    Enable HLS accelerate features
              [ ] Enable PLS stream
	      Application configuration  --->
              [*] Enable app
              [*] Enable listplayer app   
```

开发者可以根据需要选择网络播放、列表播放app、提示音播放app、命令例程。

保存后等待 RT-Thread 的包管理器自动更新完成，或者使用 `pkgs --update` 命令手动更新包到 BSP 中。

## 3、注意事项

- 注意 player 需要的auido驱动和目前RTT的audio框架下实现驱动是不兼容的, 想要使用先参考内部项目. 

## 4、联系方式

* 维护：RT-Thread 团队
