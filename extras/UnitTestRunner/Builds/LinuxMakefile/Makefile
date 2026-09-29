# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef PKG_CONFIG
  PKG_CONFIG=pkg-config
endif

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_PROJUCER_VERSION=0x90003" "-DJUCE_MODULE_AVAILABLE_juce_analytics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors_headless=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1" "-DJUCE_MODULE_AVAILABLE_juce_box2d=1" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_cryptography=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_dsp=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_javascript=1" "-DJUCE_MODULE_AVAILABLE_juce_midi_ci=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_MODULE_AVAILABLE_juce_osc=1" "-DJUCE_MODULE_AVAILABLE_juce_product_unlocking=1" "-DJUCE_WEBVIEW_INTEROP_LIBRARY_VERSION=\"1.0.0\"" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_PLUGINHOST_VST3=1" "-DJUCE_PLUGINHOST_LV2=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" "-DJUCE_UNIT_TESTS=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags $(shell ($(PKG_CONFIG) --exists webkit2gtk-4.1 && echo webkit2gtk-4.1) || echo webkit2gtk-4.0) alsa freetype2 fontconfig egl gl libcurl gtk+-x11-3.0) -pthread -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/lilv/src -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/lilv -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/sratom -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/sord/src -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/sord -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/serd -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/lv2 -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK -I../../../../modules/juce_audio_processors_headless/format_types/VST3_SDK -I../../JuceLibraryCode -I../../../../modules -I../../../../examples/Utilities/Box2D -I../../../../examples/Utilities/Box2D/Upstream/samples -I../../../../examples/Utilities/Box2D/Upstream/shared $(CPPFLAGS)
  JUCE_CPPFLAGS_CONSOLEAPP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_CONSOLEAPP := UnitTestRunner

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs alsa freetype2 fontconfig egl gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_CONSOLEAPP) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_PROJUCER_VERSION=0x90003" "-DJUCE_MODULE_AVAILABLE_juce_analytics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors_headless=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1" "-DJUCE_MODULE_AVAILABLE_juce_box2d=1" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_cryptography=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_dsp=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_javascript=1" "-DJUCE_MODULE_AVAILABLE_juce_midi_ci=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_MODULE_AVAILABLE_juce_osc=1" "-DJUCE_MODULE_AVAILABLE_juce_product_unlocking=1" "-DJUCE_WEBVIEW_INTEROP_LIBRARY_VERSION=\"1.0.0\"" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_PLUGINHOST_VST3=1" "-DJUCE_PLUGINHOST_LV2=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" "-DJUCE_UNIT_TESTS=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags $(shell ($(PKG_CONFIG) --exists webkit2gtk-4.1 && echo webkit2gtk-4.1) || echo webkit2gtk-4.0) alsa freetype2 fontconfig egl gl libcurl gtk+-x11-3.0) -pthread -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/lilv/src -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/lilv -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/sratom -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/sord/src -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/sord -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/serd -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK/lv2 -I../../../../modules/juce_audio_processors_headless/format_types/LV2_SDK -I../../../../modules/juce_audio_processors_headless/format_types/VST3_SDK -I../../JuceLibraryCode -I../../../../modules -I../../../../examples/Utilities/Box2D -I../../../../examples/Utilities/Box2D/Upstream/samples -I../../../../examples/Utilities/Box2D/Upstream/shared $(CPPFLAGS)
  JUCE_CPPFLAGS_CONSOLEAPP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_CONSOLEAPP := UnitTestRunner

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs alsa freetype2 fontconfig egl gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_CONSOLEAPP) $(JUCE_OBJDIR)
endif

OBJECTS_CONSOLEAPP := \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/Box2DSampleHost_test_e7e5d514.o \
  $(JUCE_OBJDIR)/Camera_2fc52e57.o \
  $(JUCE_OBJDIR)/Catalog_14c45af1.o \
  $(JUCE_OBJDIR)/Canvas_feeb8f0a.o \
  $(JUCE_OBJDIR)/Context_4aab7587.o \
  $(JUCE_OBJDIR)/ControlPanel_260e6f19.o \
  $(JUCE_OBJDIR)/DrawList_e9c7f974.o \
  $(JUCE_OBJDIR)/HomeViews_aea47be7.o \
  $(JUCE_OBJDIR)/HostControlsBridge_b745f859.o \
  $(JUCE_OBJDIR)/HostDrawBridge_c087efe7.o \
  $(JUCE_OBJDIR)/MetricsComponent_e82507ec.o \
  $(JUCE_OBJDIR)/ReplayFileIO_19cb31db.o \
  $(JUCE_OBJDIR)/ReplaySample_ca17d3e3.o \
  $(JUCE_OBJDIR)/Runtime_7dcad330.o \
  $(JUCE_OBJDIR)/Box2DSample_2863ffbf.o \
  $(JUCE_OBJDIR)/UpstreamBridge_b4de33f6.o \
  $(JUCE_OBJDIR)/Box2DDemoHost_d72c87c0.o \
  $(JUCE_OBJDIR)/sample_translations_1473aa95.o \
  $(JUCE_OBJDIR)/draw_stub_38886101.o \
  $(JUCE_OBJDIR)/car_6cb1448c.o \
  $(JUCE_OBJDIR)/doohickey_4b8b6dd9.o \
  $(JUCE_OBJDIR)/donut_ee67c81a.o \
  $(JUCE_OBJDIR)/dynamic_mover_b1983539.o \
  $(JUCE_OBJDIR)/geometric_mover_beca39fb.o \
  $(JUCE_OBJDIR)/sample_9808747c.o \
  $(JUCE_OBJDIR)/sample_benchmark_7f26343a.o \
  $(JUCE_OBJDIR)/sample_bodies_a63bc26d.o \
  $(JUCE_OBJDIR)/sample_character_df866.o \
  $(JUCE_OBJDIR)/sample_collision_cd2e57ef.o \
  $(JUCE_OBJDIR)/sample_continuous_9803703c.o \
  $(JUCE_OBJDIR)/sample_determinism_8884c82e.o \
  $(JUCE_OBJDIR)/sample_events_1af02546.o \
  $(JUCE_OBJDIR)/sample_geometry_d61f5a1f.o \
  $(JUCE_OBJDIR)/sample_issues_364bfe67.o \
  $(JUCE_OBJDIR)/sample_joints_72050556.o \
  $(JUCE_OBJDIR)/sample_replay_96e87514.o \
  $(JUCE_OBJDIR)/sample_restitution_c5190727.o \
  $(JUCE_OBJDIR)/sample_robustness_257bfc35.o \
  $(JUCE_OBJDIR)/sample_shapes_be5549f.o \
  $(JUCE_OBJDIR)/sample_stacking_e6ba8607.o \
  $(JUCE_OBJDIR)/sample_world_7b9bc04f.o \
  $(JUCE_OBJDIR)/benchmarks_6411c7e.o \
  $(JUCE_OBJDIR)/human_7bd8df0f.o \
  $(JUCE_OBJDIR)/utils_29a1bd93.o \
  $(JUCE_OBJDIR)/include_juce_analytics_f8e9fa94.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_flac_1_32fae422.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_flac_2_32fae7e3.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_ogg_ccb28111.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_opus_ca23cec7.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_opusfile_c2979c43.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_headless_1902bafc.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_headless_ara_8deeb88d.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_headless_lv2_libs_36180e32.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o \
  $(JUCE_OBJDIR)/include_juce_box2d_b0305d8b.o \
  $(JUCE_OBJDIR)/include_juce_box2d_upstream_938c5f19.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_core_CompilationTime_9257742c.o \
  $(JUCE_OBJDIR)/include_juce_core_zlib_ac61cd39.o \
  $(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_dsp_aeb2060f.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_graphics_Harfbuzz_60c52ba2.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libjpg_1_ce62d530.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libjpg_2_ce62d8f1.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libjpg_3_ce62dcb2.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libpng_6e070086.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dec_356f296c.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_dec_df80faee.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_dec_avx2_258a73aa.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_dec_sse41_3dcad1b1.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_enc_df9329c6.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_enc_avx2_503bcbd2.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_enc_sse41_69447e89.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_enc_35815844.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_sharpyuv_ee7d37d0.o \
  $(JUCE_OBJDIR)/include_juce_graphics_libwebp_utils_339e1d1b.o \
  $(JUCE_OBJDIR)/include_juce_graphics_lunasvg_92e75770.o \
  $(JUCE_OBJDIR)/include_juce_graphics_Sheenbidi_c310974d.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_2_153c9db8.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_3_154ab539.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_4_1558ccba.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_5_1566e43b.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_javascript_84e88969.o \
  $(JUCE_OBJDIR)/include_juce_midi_ci_1fda4092.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \
  $(JUCE_OBJDIR)/include_juce_osc_f3df604d.o \
  $(JUCE_OBJDIR)/include_juce_product_unlocking_8278fcdc.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_CONSOLEAPP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_CONSOLEAPP) : $(OBJECTS_CONSOLEAPP) $(JUCE_OBJDIR)/execinfo.cmd $(RESOURCES)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors alsa freetype2 fontconfig egl gl libcurl
	@echo Linking "UnitTestRunner - ConsoleApp"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_CONSOLEAPP) $(OBJECTS_CONSOLEAPP) $(JUCE_LDFLAGS) $(shell cat $(JUCE_OBJDIR)/execinfo.cmd) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Box2DSampleHost_test_e7e5d514.o: ../../../../examples/Utilities/Box2D/Box2DSampleHost_test.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Box2DSampleHost_test.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Camera_2fc52e57.o: ../../../../examples/Utilities/Box2D/Camera.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Camera.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Catalog_14c45af1.o: ../../../../examples/Utilities/Box2D/Catalog.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Catalog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Canvas_feeb8f0a.o: ../../../../examples/Utilities/Box2D/Canvas.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Canvas.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Context_4aab7587.o: ../../../../examples/Utilities/Box2D/Context.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Context.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ControlPanel_260e6f19.o: ../../../../examples/Utilities/Box2D/ControlPanel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ControlPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DrawList_e9c7f974.o: ../../../../examples/Utilities/Box2D/DrawList.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DrawList.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HomeViews_aea47be7.o: ../../../../examples/Utilities/Box2D/HomeViews.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling HomeViews.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HostControlsBridge_b745f859.o: ../../../../examples/Utilities/Box2D/HostControlsBridge.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling HostControlsBridge.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HostDrawBridge_c087efe7.o: ../../../../examples/Utilities/Box2D/HostDrawBridge.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling HostDrawBridge.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MetricsComponent_e82507ec.o: ../../../../examples/Utilities/Box2D/MetricsComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MetricsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ReplayFileIO_19cb31db.o: ../../../../examples/Utilities/Box2D/ReplayFileIO.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ReplayFileIO.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ReplaySample_ca17d3e3.o: ../../../../examples/Utilities/Box2D/ReplaySample.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ReplaySample.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Runtime_7dcad330.o: ../../../../examples/Utilities/Box2D/Runtime.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Runtime.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Box2DSample_2863ffbf.o: ../../../../examples/Utilities/Box2D/Box2DSample.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Box2DSample.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UpstreamBridge_b4de33f6.o: ../../../../examples/Utilities/Box2D/UpstreamBridge.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling UpstreamBridge.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Box2DDemoHost_d72c87c0.o: ../../../../examples/Utilities/Box2D/Box2DDemoHost.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Box2DDemoHost.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_translations_1473aa95.o: ../../../../examples/Utilities/Box2D/Upstream/sample_translations.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_translations.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/draw_stub_38886101.o: ../../../../examples/Utilities/Box2D/Upstream/samples/draw_stub.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling draw_stub.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/car_6cb1448c.o: ../../../../examples/Utilities/Box2D/Upstream/samples/car.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling car.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/doohickey_4b8b6dd9.o: ../../../../examples/Utilities/Box2D/Upstream/samples/doohickey.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling doohickey.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/donut_ee67c81a.o: ../../../../examples/Utilities/Box2D/Upstream/samples/donut.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling donut.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dynamic_mover_b1983539.o: ../../../../examples/Utilities/Box2D/Upstream/samples/dynamic_mover.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling dynamic_mover.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/geometric_mover_beca39fb.o: ../../../../examples/Utilities/Box2D/Upstream/samples/geometric_mover.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling geometric_mover.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_9808747c.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_benchmark_7f26343a.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_benchmark.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_benchmark.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_bodies_a63bc26d.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_bodies.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_bodies.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_character_df866.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_character.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_character.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_collision_cd2e57ef.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_collision.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_collision.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_continuous_9803703c.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_continuous.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_continuous.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_determinism_8884c82e.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_determinism.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_determinism.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_events_1af02546.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_events.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_geometry_d61f5a1f.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_geometry.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_geometry.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_issues_364bfe67.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_issues.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_issues.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_joints_72050556.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_joints.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_joints.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_replay_96e87514.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_replay.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_replay.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_restitution_c5190727.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_restitution.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_restitution.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_robustness_257bfc35.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_robustness.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_robustness.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_shapes_be5549f.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_shapes.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_shapes.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_stacking_e6ba8607.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_stacking.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_stacking.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/sample_world_7b9bc04f.o: ../../../../examples/Utilities/Box2D/Upstream/samples/sample_world.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling sample_world.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/benchmarks_6411c7e.o: ../../../../examples/Utilities/Box2D/Upstream/shared/benchmarks.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling benchmarks.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/human_7bd8df0f.o: ../../../../examples/Utilities/Box2D/Upstream/shared/human.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling human.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/utils_29a1bd93.o: ../../../../examples/Utilities/Box2D/Upstream/shared/utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_analytics_f8e9fa94.o: ../../JuceLibraryCode/include_juce_analytics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_analytics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o: ../../JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o: ../../JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o: ../../JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_flac_1_32fae422.o: ../../JuceLibraryCode/include_juce_audio_formats_flac_1.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats_flac_1.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_flac_2_32fae7e3.o: ../../JuceLibraryCode/include_juce_audio_formats_flac_2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats_flac_2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_ogg_ccb28111.o: ../../JuceLibraryCode/include_juce_audio_formats_ogg.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats_ogg.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_opus_ca23cec7.o: ../../JuceLibraryCode/include_juce_audio_formats_opus.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats_opus.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_opusfile_c2979c43.o: ../../JuceLibraryCode/include_juce_audio_formats_opusfile.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats_opusfile.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o: ../../JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_headless_1902bafc.o: ../../JuceLibraryCode/include_juce_audio_processors_headless.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_headless.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_headless_ara_8deeb88d.o: ../../JuceLibraryCode/include_juce_audio_processors_headless_ara.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_headless_ara.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_headless_lv2_libs_36180e32.o: ../../JuceLibraryCode/include_juce_audio_processors_headless_lv2_libs.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_headless_lv2_libs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o: ../../JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_box2d_b0305d8b.o: ../../JuceLibraryCode/include_juce_box2d.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_box2d.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_box2d_upstream_938c5f19.o: ../../JuceLibraryCode/include_juce_box2d_upstream.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_box2d_upstream.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_CompilationTime_9257742c.o: ../../JuceLibraryCode/include_juce_core_CompilationTime.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core_CompilationTime.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_zlib_ac61cd39.o: ../../JuceLibraryCode/include_juce_core_zlib.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core_zlib.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o: ../../JuceLibraryCode/include_juce_cryptography.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_cryptography.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_dsp_aeb2060f.o: ../../JuceLibraryCode/include_juce_dsp.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_dsp.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_Harfbuzz_60c52ba2.o: ../../JuceLibraryCode/include_juce_graphics_Harfbuzz.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_Harfbuzz.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libjpg_1_ce62d530.o: ../../JuceLibraryCode/include_juce_graphics_libjpg_1.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libjpg_1.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libjpg_2_ce62d8f1.o: ../../JuceLibraryCode/include_juce_graphics_libjpg_2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libjpg_2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libjpg_3_ce62dcb2.o: ../../JuceLibraryCode/include_juce_graphics_libjpg_3.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libjpg_3.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libpng_6e070086.o: ../../JuceLibraryCode/include_juce_graphics_libpng.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libpng.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dec_356f296c.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_dec_df80faee.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dsp_dec.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dsp_dec.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_dec_avx2_258a73aa.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dsp_dec_avx2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dsp_dec_avx2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_dec_sse41_3dcad1b1.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dsp_dec_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dsp_dec_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_enc_df9329c6.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dsp_enc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dsp_enc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_enc_avx2_503bcbd2.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dsp_enc_avx2.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dsp_enc_avx2.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_dsp_enc_sse41_69447e89.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_dsp_enc_sse41.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_dsp_enc_sse41.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_enc_35815844.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_enc.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_enc.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_sharpyuv_ee7d37d0.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_sharpyuv.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_sharpyuv.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_libwebp_utils_339e1d1b.o: ../../JuceLibraryCode/include_juce_graphics_libwebp_utils.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_libwebp_utils.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_lunasvg_92e75770.o: ../../JuceLibraryCode/include_juce_graphics_lunasvg.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_lunasvg.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_Sheenbidi_c310974d.o: ../../JuceLibraryCode/include_juce_graphics_Sheenbidi.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_Sheenbidi.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_2_153c9db8.o: ../../JuceLibraryCode/include_juce_gui_basics_2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_3_154ab539.o: ../../JuceLibraryCode/include_juce_gui_basics_3.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_4_1558ccba.o: ../../JuceLibraryCode/include_juce_gui_basics_4.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_5_1566e43b.o: ../../JuceLibraryCode/include_juce_gui_basics_5.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics_5.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_javascript_84e88969.o: ../../JuceLibraryCode/include_juce_javascript.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_javascript.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_midi_ci_1fda4092.o: ../../JuceLibraryCode/include_juce_midi_ci.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_midi_ci.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o: ../../JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_osc_f3df604d.o: ../../JuceLibraryCode/include_juce_osc.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_osc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_product_unlocking_8278fcdc.o: ../../JuceLibraryCode/include_juce_product_unlocking.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_product_unlocking.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_CONSOLEAPP) $(JUCE_CFLAGS_CONSOLEAPP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/execinfo.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link libexecinfo"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/execinfo.x -lexecinfo - >/dev/null 2>&1 && printf -- "-lexecinfo" > "$@" || touch "$@"

$(JUCE_OBJDIR)/cxxfs.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link stdc++fs"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/cxxfs.x -lstdc++fs - >/dev/null 2>&1 && printf -- "-lstdc++fs" > "$@" || touch "$@"

clean:
	@echo Cleaning UnitTestRunner
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping UnitTestRunner
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_CONSOLEAPP)

-include $(OBJECTS_CONSOLEAPP:%.o=%.d)
