// Copyright 2012 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SYZYGY_PDB_PDB_UTIL_H_
#define SYZYGY_PDB_PDB_UTIL_H_

#include <windows.h>  // NOLINT
#include <dbghelp.h>
#include <map>
#include <vector>

#include "base/file_path.h"
#include "syzygy/pdb/pdb_data.h"

namespace pdb {

// Forward declare.
class PdbStream;
class WritablePdbStream;

// A map of names to stream IDs, stored in the header stream.
typedef std::map<std::string, uint32> NameStreamMap;

// Used for parsing a variable sized bitset as found in PDB streams.
class PdbBitSet {
 public:
  // Reads a bit set from the given stream at its current cursor position.
  // @param stream the stream to be read.
  // @returns true on success, false otherwise.
  bool Read(PdbStream* stream);

  // Writes a bit set to the given stream at its current cursor position.
  bool Write(WritablePdbStream* stream);

  // Resizes the given bit set. Will be the next multiple of 32 in size.
  // @param bits the minimum number of bits to hold.
  void Resize(size_t bits);

  // Sets the given bit.
  void Set(size_t bit);

  // Clears the given bit.
  void Clear(size_t bit);

  // Toggles the given bit.
  void Toggle(size_t bit);

  // Determines if a given bit is set.
  // @param bit the position of the bit to inspect.
  // @returns true if the bit at position @p bit is set.
  bool IsSet(size_t bit) const;

  // @returns true if the bit set contains no data.
  bool IsEmpty() const;

  // @returns the number of bits in the bit set.
  size_t size() const { return bits_.size() * 32; }

 private:
  std::vector<uint32> bits_;
};

// Get the DbiDbgHeader offset within the Dbi info stream. For some reason,
// the EC info data comes before the Dbi debug header despite that the Dbi
// debug header size comes before the EC info size in the Dbi header struct.
uint32 GetDbiDbgHeaderOffset(const DbiHeader& dbi_header);

// Add Omap stream data to an existing Pdb file and write it as a new Pdb file,
// while updating the Pdb header to a new GUID and timestamp.
// The Omap vector arguments must already be sorted in ascending order by rva.
// @param output_guid a new GUID to assign to the output_file.
bool AddOmapStreamToPdbFile(const FilePath& input_file,
                            const FilePath& output_file,
                            const GUID& output_guid,
                            const std::vector<OMAP>& omap_to_list,
                            const std::vector<OMAP>& omap_from_list);

// Reads the header from the given PDB file @p pdb_path.
// @param pdb_path the path to the PDB whose header is to be read.
// @param pdb_header the header to be filled in.
bool ReadPdbHeader(const FilePath& pdb_path, PdbInfoHeader70* pdb_header);

// Reads the header from the given PDB file @p pdb_path.
// @param pdb_stream the stream containing the header.
// @param pdb_header the header to be filled in.
// @param name_stream_map the name-stream map to be filled in.
// @returns true on success, false on error.
bool ReadHeaderInfoStream(PdbStream* pdb_stream,
                          PdbInfoHeader70* pdb_header,
                          NameStreamMap* name_stream_map);

}  // namespace pdb

#endif  // SYZYGY_PDB_PDB_UTIL_H_