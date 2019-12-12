/* -*- c++ -*- */
/* 
 * Copyright 2019 Wojciech Kazubski.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "gnmax_source.h"
#include "gnmax_source_cc_impl.h"

namespace gr {
  namespace gnMAX2769 {

    gnmax_source_cc::sptr
    gnmax_source_cc::make(int bias, int ant, float freq, int bw, int zeroif)
    {
      return gnuradio::get_initial_sptr
        (new gnmax_source_cc_impl(bias, ant, freq, bw, zeroif));
    }

    /*
     * The private constructor
     */
    gnmax_source_cc_impl::gnmax_source_cc_impl(int bias, int ant, float freq, int bw, int zeroif)
      : gr::sync_block("gnmax_source_cc",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      // constructor code here
      variables.bias = bias;
      variables.ant = ant;
      if (zeroif != 0)
        variables.zeroif = 4;
      else
        variables.zeroif = 0;
      variables.freq = static_cast<int>(freq / 1023000 + 0.5) - 4 + variables.zeroif;
      variables.bw = bw;
      gnmax_drv = new gnmax_Source(variables);
      fprintf(stdout,"GNMAX Start\n");
    }

    /*
     * Our virtual destructor.
     */
    gnmax_source_cc_impl::~gnmax_source_cc_impl()
    {
      // destructor code here
      if(gnmax_drv != NULL)
      {
        fprintf(stdout,"Destructing MAX2769\n");
        delete gnmax_drv;
        //delete packet;
      }
    }

    int
    gnmax_source_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>
      int n_samples_rx;

      if (noutput_items<=GNMAX_SAMPS_5MS)
      {
        n_samples_rx = gnmax_drv->Read(&packet,noutput_items);
      }
      else
      {
        n_samples_rx = gnmax_drv->Read(&packet,GNMAX_SAMPS_5MS);
      }
      for (int i = 0; i < n_samples_rx; i++)
      {
        out[i] = gr_complex(packet.data[i].i, packet.data[i].q);
      }

      // Tell runtime system how many output items we produced.
      return n_samples_rx;
    }

    void gnmax_source_cc_impl::set_bias(int bias)
    {
      gnmax_drv->w_set_bias(bias);
      variables.bias = bias;
    }

    void gnmax_source_cc_impl::set_ant(int ant)
    {
      gnmax_drv->w_set_ant(ant);
      variables.ant = ant;
    }

    void gnmax_source_cc_impl::set_freq(float freq)
    {
      int f = static_cast<int>(freq / 1023000 + 0.5) - 4 + variables.zeroif;
      gnmax_drv->w_set_freq(f);
      variables.freq = f;
    }

    void gnmax_source_cc_impl::set_bw(int bw)
    {
      gnmax_drv->w_set_bw(bw, variables.zeroif);
      variables.bw = bw;
    }

  } /* namespace gnMAX2769 */
} /* namespace gr */

