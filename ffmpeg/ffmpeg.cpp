#include <avcpp/av.h>
#include <avcpp/ffmpeg.h>
#include <avcpp/codec.h>
#include <avcpp/packet.h>
#include <avcpp/videorescaler.h>
#include <avcpp/audioresampler.h>
#include <avcpp/avutils.h>

// API2
#include <avcpp/format.h>
#include <avcpp/formatcontext.h>
#include <avcpp/codec.h>
#include <avcpp/codeccontext.h>

namespace Convertor {

int convertAudio(std::string uri, std::string out) {
	av::init();
	av::setFFmpegLoggingLevel(AV_LOG_TRACE);

	ssize_t      audioStream = -1;
	av::AudioDecoderContext adec;
	av::Stream   ast;
	std::error_code   ec;

	int count = 0;

	//
	// INPUT
	//
	av::FormatContext ictx;

	ictx.openInput(uri, ec);
	if (ec) {
		std::cerr << "Can't open input\n";
	    return 1;
	}

	ictx.findStreamInfo();

	for (size_t i = 0; i < ictx.streamsCount(); ++i) {
	    auto st = ictx.stream(i);
	    if (st.isAudio()) {
		audioStream = i;
		ast = st;
		break;
	    }
	}

	std::cerr << audioStream << std::endl;

	if (ast.isNull()) {
		std::cerr << "Audio stream not found\n";
	    return 1;
	}

	if (ast.isValid()) {
	    adec = av::AudioDecoderContext(ast);

	    //Codec codec = findDecodingCodec(adec.raw()->codec_id);

	    //adec.setCodec(codec);
	    //adec.setRefCountedFrames(true);

	    adec.open(ec);

	    if (ec) {
		    std::cerr << "Can't open codec\n";
		return 1;
	    }
	}

	//
	// OUTPUT
	//
	av::OutputFormat  ofmt;
	av::FormatContext octx;

	ofmt = av::guessOutputFormat(out, out);
	//std::clog << "Output format: " << ofmt.name() << " / " << ofmt.longName() << '\n';
	octx.setFormat(ofmt);

	av::Codec        ocodec = av::findEncodingCodec(ofmt, false);
	av::Stream      ost    = octx.addStream(ocodec);
	av::AudioEncoderContext enc (ost);

	//std::clog << ocodec.name() << " / " << ocodec.longName() << ", audio: " << (ocodec.type()==AVMEDIA_TYPE_AUDIO) << '\n';

	auto sampleFmts  = ocodec.supportedSampleFormats();
	auto sampleRates = ocodec.supportedSamplerates();
	auto layouts     = ocodec.supportedChannelLayouts();

	//std::clog << "Supported sample formats:\n";
	for (const auto &fmt : sampleFmts) {
		//std::clog << "  " << av_get_sample_fmt_name(fmt) << '\n';
	}

	//std::clog << "Supported sample rates:\n";
	for (const auto &rate : sampleRates) {
		//std::clog << "  " << rate << '\n';
	}

	//std::clog << "Supported sample layouts:\n";
	for (const auto &lay : layouts) {
	    char buf[128] = {0};
	    av_get_channel_layout_string(buf,
					 sizeof(buf),
					 av_get_channel_layout_nb_channels(lay),
					 lay);

	    //std::clog << "  " << buf << '\n';
	}

	//return 0;


	// Settings
#if 1
	enc.setSampleRate(44100);
	enc.setSampleFormat(sampleFmts[0]);
	// Layout
	//enc.setChannelLayout(adec.channelLayout());
	enc.setChannelLayout(AV_CH_LAYOUT_STEREO);
	//enc.setChannelLayout(AV_CH_LAYOUT_MONO);
	enc.setTimeBase(av::Rational(1, enc.sampleRate()));
	enc.setBitRate(adec.bitRate());
#else
	enc.setSampleRate(adec.sampleRate());
	enc.setSampleFormat(adec.sampleFormat());
	enc.setChannelLayout(adec.channelLayout());
	enc.setTimeBase(adec.timeBase());
	enc.setBitRate(adec.bitRate());
#endif

	octx.openOutput(out, ec);
	if (ec) {
		std::cerr << "Can't open output\n";
	    return 1;
	}

	enc.open(ec);
	if (ec) {
		std::cerr << "Can't open encoder\n";
	    return 1;
	}

	///std::clog << "Encoder frame size: " << enc.frameSize() << '\n';

	octx.dump();
	octx.writeHeader();
	octx.flush();

	//
	// RESAMPLER
	//
	av::AudioResampler resampler(enc.channelLayout(),  enc.sampleRate(),  enc.sampleFormat(),
				 adec.channelLayout(), adec.sampleRate(), adec.sampleFormat());

	//
	// PROCESS
	//
	while (true) {
	    av::Packet pkt = ictx.readPacket(ec);
	    if (ec)
	    {
		    std::clog << "Packet reading error: " << ec << ", " << ec.message() << std::endl;
		break;
	    }

	    if (pkt.streamIndex() != audioStream) {
		continue;
	    }

	    //std::clog << "Read packet: isNull=" << (bool)!pkt << ", " << pkt.pts() << "(nopts:" << pkt.pts().isNoPts() << ")" << " / " << pkt.pts().seconds() << " / " << pkt.timeBase() << " / st: " << pkt.streamIndex() << std::endl;
#if 0
	    if (pkt.pts() == av::NoPts && pkt.timeBase() == Rational())
	    {
		clog << "Skip invalid timestamp packet: data=" << (void*)pkt.data()
		     << ", size=" << pkt.size()
		     << ", flags=" << pkt.flags() << " (corrupt:" << (pkt.flags() & AV_PKT_FLAG_CORRUPT) << ";key:" << (pkt.flags() & AV_PKT_FLAG_KEY) << ")"
		     << ", side_data=" << (void*)pkt.raw()->side_data
		     << ", side_data_count=" << pkt.raw()->side_data_elems
		     << std::endl;
		//continue;
	    }
#endif

	    auto samples = adec.decode(pkt, ec);
	    count++;
	    //if (count > 200)
	    //    break;

	    if (ec) {
		    std::cerr << "Decode error: " << ec << ", " << ec.message() << std::endl;
		return 1;
	    } else if (!samples) {
		    std::cerr << "Empty samples set\n";

		//if (!pkt) // decoder flushed here
		//   break;
		//continue;
	    }

	    /**std::clog << "  Samples [in]: " << samples.samplesCount()
		 << ", ch: " << samples.channelsCount()
		 << ", freq: " << samples.sampleRate()
		 << ", name: " << samples.channelsLayoutString()
		 << ", pts: " << samples.pts().seconds()
		 << ", ref=" << samples.isReferenced() << ":" << samples.refCount()
		 << std::endl;**/

	    // Empty samples set should not be pushed to the resampler, but it is valid case for the
	    // end of reading: during samples empty, some cached data can be stored at the resampler
	    // internal buffer, so we should consume it.
	    if (samples)
	    {
		resampler.push(samples, ec);
		if (ec) {
			std::clog << "Resampler push error: " << ec << ", text: " << ec.message() << std::endl;
		    continue;
		}
	    }

	    // Pop resampler data
	    bool getAll = !samples;
	    while (true) {
		av::AudioSamples ouSamples(enc.sampleFormat(),
				       enc.frameSize(),
				       enc.channelLayout(),
				       enc.sampleRate());

		// Resample:
		bool hasFrame = resampler.pop(ouSamples, getAll, ec);
		if (ec) {
			std::clog << "Resampling status: " << ec << ", text: " << ec.message() << std::endl;
		    break;
		} else if (!hasFrame) {
		    break;
		} else
		    /**std::clog << "  Samples [ou]: " << ouSamples.samplesCount()
			 << ", ch: " << ouSamples.channelsCount()
			 << ", freq: " << ouSamples.sampleRate()
			 << ", name: " << ouSamples.channelsLayoutString()
			 << ", pts: " << ouSamples.pts().seconds()
			 << ", ref=" << ouSamples.isReferenced() << ":" << ouSamples.refCount()
			 << std::endl;**/

		// ENCODE
		ouSamples.setStreamIndex(0);
		ouSamples.setTimeBase(enc.timeBase());

		av::Packet opkt = enc.encode(ouSamples, ec);
		if (ec) {
			std::cerr << "Encoding error: " << ec << ", " << ec.message() << std::endl;
		    return 1;
		} else if (!opkt) {
		    //std::cerr << "Empty packet\n";
		    continue;
		}

		opkt.setStreamIndex(0);

		//std::clog << "Write packet: pts=" << opkt.pts() << ", dts=" << opkt.dts() << " / " << opkt.pts().seconds() << " / " << opkt.timeBase() << " / st: " << opkt.streamIndex() << std::endl;

		octx.writePacket(opkt, ec);
		if (ec) {
			std::cerr << "Error write packet: " << ec << ", " << ec.message() << std::endl;
		    return 1;
		}
	    }

	    // For the first packets samples can be empty: decoder caching
	    if (!pkt && !samples)
		break;
	}

	//
	// Is resampler flushed?
	//
	std::cerr << "Delay: " << resampler.delay() << std::endl;

	//
	// Flush encoder queue
	//
	//std::clog << "Flush encoder:\n";
	while (true) {
	    av::AudioSamples null(nullptr);
	    av::Packet        opkt = enc.encode(null, ec);
	    if (ec || !opkt)
		break;

	    opkt.setStreamIndex(0);

	    //std::clog << "Write packet: pts=" << opkt.pts() << ", dts=" << opkt.dts() << " / " << opkt.pts().seconds() << " / " << opkt.timeBase() << " / st: " << opkt.streamIndex() << std::endl;

	    octx.writePacket(opkt, ec);
	    if (ec) {
		    std::cerr << "Error write packet: " << ec << ", " << ec.message() << std::endl;
		return 1;
	    }

	}

	octx.flush();
	octx.writeTrailer();

	return 0;
}

int addAudioToVideo(std::string audio, std::string video, std::string output) {
	av::init();
	av::setFFmpegLoggingLevel(AV_LOG_DEBUG);

	std::error_code ec;

	// Open input video

	av::FormatContext ictxVideo;
	ssize_t      videoStream = -1;
	av::VideoDecoderContext vdec;
	av::Stream      vst;

	ictxVideo.openInput(video, ec);

	if (ec) {
		std::cerr << "Can't open input\n";
		return 1;
	}

	ictxVideo.findStreamInfo();

	for (size_t i = 0; i < ictxVideo.streamsCount(); ++i) {
		auto st = ictxVideo.stream(i);

		if (st.mediaType() == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			vst = st;
			break;
		}
	}

	if (vst.isNull()) {
		std::cerr << "Video stream not found\n";
		return 1;
	}

	if (vst.isValid()) {
		vdec = av::VideoDecoderContext(vst);
		vdec.setRefCountedFrames(true);

		vdec.open(av::Codec(), ec);
		if (ec) {
			std::cerr << "Can't open codec\n";
			return 1;
		}
	}

	// Open input audio
	
	av::FormatContext ictxAudio;
	ssize_t      audioStream = -1;
	av::AudioDecoderContext adec;
	av::Stream   ast;

	ictxAudio.openInput(audio, ec);
	if (ec) {
		std::cerr << "Can't open input\n";
	    return 1;
	}

	ictxAudio.findStreamInfo();

	for (size_t i = 0; i < ictxAudio.streamsCount(); ++i) {
	    auto st = ictxAudio.stream(i);
	    if (st.isAudio()) {
		audioStream = i;
		ast = st;
		break;
	    }
	}

	std::cerr << audioStream << std::endl;

	if (ast.isNull()) {
		std::cerr << "Audio stream not found\n";
	    return 1;
	}

	if (ast.isValid()) {
	    adec = av::AudioDecoderContext(ast);

	    //Codec codec = findDecodingCodec(adec.raw()->codec_id);

	    //adec.setCodec(codec);
	    //adec.setRefCountedFrames(true);

	    adec.open(ec);

	    if (ec) {
		    std::cerr << "Can't open codec\n";
		return 1;
	    }
	}

	// Setup the output video
	
	av::OutputFormat  ofrmt;
	av::FormatContext octx;

	av::Codec        ocodec  = findEncodingCodec(ofrmt);
	av::Stream      ost     = octx.addStream(ocodec);
	av::VideoEncoderContext encoder {ost};

	ofrmt.setFormat(std::string(), output);
	octx.setFormat(ofrmt);

	/// Setings

	encoder.setWidth(vdec.width());
	encoder.setHeight(vdec.height());

	if (vdec.pixelFormat() > -1)
		encoder.setPixelFormat(vdec.pixelFormat());

	encoder.setTimeBase(av::Rational{1, 1000});
	encoder.setBitRate(vdec.bitRate());
	ost.setFrameRate(vst.frameRate());

	octx.openOutput(output, ec);
	if (ec) {
		std::cerr << "Can't open output\n";
		return 1;
	}

	encoder.open(av::Codec(), ec);
	if (ec) {
		std::cerr << "Can't opent encodec\n";
		return 1;
	}

	octx.dump();
	octx.writeHeader();
	octx.flush();


	while (true) {
		break;
	}


	octx.writeTrailer();

	return 0;
}

}
