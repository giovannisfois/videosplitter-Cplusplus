#include "in_audio.h"
#include <string>
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}


std::ostream &operator<<(std::ostream &stream, in_audio *audio){
  /* Print detailed information about the input or output format, 
     such as duration, bitrate, streams, container, programs, metadata, side data, 
     codec and time base. 
  */

  stream << "-------------------------" << std::endl;
  stream << "Stream " << av_get_media_type_string(audio->type) << " Id :" << audio->stream_id << std::endl;
  
  return stream;
}


int in_audio::readFrame(){
  
  int input_audio_stream_id    = this->stream_id;
  AVFormatContext *pFormatCtx  = this->av_format_context; 

  int result = 0;
  int frameFinished;
  int decoded = 0;
  
  
  while(av_read_frame(pFormatCtx, &(this->packet))>=0){
    //The packet now contains some of our input stream data - we need to fill the correct fields
    
    if(this->packet.stream_index == input_audio_stream_id) {
      result = avcodec_decode_audio4(this->av_format_context->streams[input_audio_stream_id]->codec,
				     this->pFrame,
				     &frameFinished,
				     &(this->packet));

      /* Some audio decoders decode only part of the packet, and have to be
       * called again with the remainder of the packet data.
       * Sample: fate-suite/lossless-audio/luckynight-partial.shn
       * Also, some decoders might over-read the packet. 
       */
      /* The size of the data read */
      decoded = FFMIN(result, this->packet.size);

      if(frameFinished){
	this->has_frame = 1;
	av_free_packet(&(this->packet));
	return decoded;
      }
    }

    // Free the packet that was allocated by av_read_frame
    av_free_packet(&(this->packet));
  }
  
  return decoded;
}
