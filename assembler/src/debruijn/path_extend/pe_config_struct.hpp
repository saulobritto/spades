//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

/*
 * lc_config_struct.hpp
 *
 *  Created on: Aug 16, 2011
 *      Author: Alexey.Gurevich
 */

#ifndef LC_CONFIG_STRUCT_HPP_
#define LC_CONFIG_STRUCT_HPP_

#include "config_singl.hpp"
#include "cpp_utils.hpp"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/bimap.hpp>

#include <string>
#include <vector>

namespace path_extend {

const char * const pe_cfg_filename = "./config/debruijn/path_extend/lc_config.info";

enum output_broken_scaffolds {
    obs_none,
    obs_break_gaps,
    obs_break_all
};

// struct for path extend subproject's configuration file
struct pe_config {

  typedef boost::bimap<std::string, output_broken_scaffolds> output_broken_scaffolds_id_mapping;

  static const output_broken_scaffolds_id_mapping FillOBSInfo() {
    output_broken_scaffolds_id_mapping::value_type info[] = {
              output_broken_scaffolds_id_mapping::value_type("none", obs_none),
              output_broken_scaffolds_id_mapping::value_type("break_gaps", obs_break_gaps),
              output_broken_scaffolds_id_mapping::value_type("break_all", obs_break_all)
    };

    return output_broken_scaffolds_id_mapping(info, utils::array_end(info));
  }

  static const output_broken_scaffolds_id_mapping& output_broken_scaffolds_info() {
    static output_broken_scaffolds_id_mapping output_broken_scaffolds_info = FillOBSInfo();
    return output_broken_scaffolds_info;
  }

  static const std::string& output_broken_scaffolds_name(output_broken_scaffolds obs) {
    auto it = output_broken_scaffolds_info().right.find(obs);
    VERIFY_MSG(it != output_broken_scaffolds_info().right.end(),
               "No name for working stage id = " << obs);

    return it->second;
  }

  static output_broken_scaffolds output_broken_scaffolds_id(std::string name) {
    auto it = output_broken_scaffolds_info().left.find(name);
    VERIFY_MSG(it != output_broken_scaffolds_info().left.end(),
               "There is no working stage with name = " << name);

    return it->second;
  }

  struct DatasetT {
    struct PairedLibT {
      size_t read_size;
      size_t insert_size;
      size_t var;

      std::string path;
    };

    std::string param_set;
    std::string graph_file;

    std::vector<PairedLibT> libs;
    //boost::optional<std::string> reference_genome;
  };

  struct OutputParamsT {
    bool write_overlaped_paths;
    bool write_paths;
    bool write_path_loc;

    void DisableAll() {
      write_overlaped_paths = false;
      write_paths = false;
      write_path_loc = false;
    }
  };

  struct VisualizeParamsT {
    bool print_overlaped_paths;
    bool print_paths;

    void DisableAll() {
      print_overlaped_paths = false;
      print_paths = false;
    }
  };

  struct ParamSetT {
    std::string metric;
    bool normalize_weight;
    bool normalize_by_coverage;

    bool improve_paired_info;

    size_t split_edge_length;

    struct ExtensionOptionsT {
      std::string metric;

      bool try_deep_search;

      struct SelectOptionsT {
        boost::optional<double> single_threshold;
        double weight_threshold;
        double priority_coeff;

        SelectOptionsT() {}
        SelectOptionsT(const SelectOptionsT& so)
            : single_threshold(so.single_threshold), weight_threshold(so.weight_threshold), priority_coeff(so.priority_coeff) {}
      } select_options;


    } extension_options;

    ExtensionOptionsT mate_pair_options;


    struct ScaffolderOptionsT {
      bool on;
      int cutoff;
      double rel_cutoff;
      double sum_threshold;

      bool cluster_info;
      double cl_threshold;

      bool fix_gaps;
      double min_gap_score;
      double max_must_overlap;
      double max_can_overlap;
      int short_overlap;
      int artificial_gap;
    } scaffolder_options;


    struct LoopRemovalT {
      bool inspect_short_loops;

      size_t max_loops;
      bool full_loop_removal;
    } loop_removal;


    struct FilterOptionsT {
      bool remove_overlaps;
    } filter_options;
  };
  struct UtilsT {
    int mode;
    std::string file1;
    std::string file2;

    std::string clustered;
    std::string advanced;
    size_t insert_size;
    size_t read_size;
    size_t dev;
  };

  struct LongReads {
	  double filtering;
	  double priority;
  };

  struct AllLongReads{
      LongReads single_reads;
      LongReads pacbio_reads;
      LongReads coverage_base_rr;
  };

  struct MainPEParamsT {
    std::string name;
    output_broken_scaffolds obs;

    bool debug_output;
    std::string etc_dir;

    OutputParamsT output;
    VisualizeParamsT viz;
    ParamSetT param_set;
    AllLongReads long_reads;
  } params;

  std::string dataset_name;
  DatasetT dataset;
};



void load(pe_config::MainPEParamsT& p, boost::property_tree::ptree const& pt, bool complete);
void load(pe_config& pe_cfg, boost::property_tree::ptree const& pt, bool complete);

}

typedef config_common::config<path_extend::pe_config> pe_cfg;

#endif /* CONFIG_STRUCT_HPP_ */
