--[[
* Copyright 2021 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the Oberon+ parser/compiler library.
*
* The following is the license that applies to this copy of the
* library. For a license to use the library under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
]]--

local ffi = require 'ffi'
local C = ffi.C
local string = require 'string'
local io = require 'io'
local math = require 'math'
local bit = require 'bit'
local os = require 'os'

local module = {}

ffi.cdef[[
    int ObxFfi_DIV( int a, int b );
    int ObxFfi_MOD( int a, int b );
    
	typedef struct{ 
		int count; 
		uint8_t data[?];
	} ByteArray;
	typedef struct{
	    int count; 
	    uint16_t data[?];
	} WordArray;
	typedef struct{
	    int count; 
	    int16_t data[?];
	} ShortArray;
	typedef struct{
	    int count; 
	    int32_t data[?];
	} IntArray;
	typedef struct{
	    int count; 
	    int64_t data[?];
	} LongArray;
	typedef struct{
	    int count; 
	    float data[?];
	} FloatArray;
	typedef struct{
	    int count; 
	    double data[?];
	} DoubleArray;

	void ObxFfi_initString( ByteArray* ba, const char* str );
	void ObxFfi_initWstring( WordArray* wa, const char* str );
	void ObxFfi_initByteArray( ByteArray* ba, const char* str );
	int ObxFfi_strRelOp( ByteArray* lhs, ByteArray* rhs, int op );
	int ObxFfi_wstrRelOp( WordArray* lhs, WordArray* rhs, int op );
]]

local ByteArray = ffi.typeof("ByteArray")
local WordArray = ffi.typeof("WordArray")
local ShortArray = ffi.typeof("ShortArray")
local IntArray = ffi.typeof("IntArray")
local LongArray = ffi.typeof("LongArray")
local FloatArray = ffi.typeof("FloatArray")
local DoubleArray = ffi.typeof("DoubleArray")
module.ByteArray = ByteArray
module.WordArray = WordArray
module.ShortArray = ShortArray
module.IntArray = IntArray
module.LongArray = LongArray
module.FloatArray = FloatArray
module.DoubleArray = DoubleArray

function module.charToStringArray(len, str)
	local a = ffi.new( ByteArray, len ) 
	a.count = len
	if str then
		C.ObxFfi_initString(a,str)
	end
	return a
end
function module.createWcharArray(len, str)
	local a = ffi.new( WordArray, len ) 
	a.count = len
	if str then
		C.ObxFfi_initWstring(a,str)
	end 
	return a
end
function module.createByteArray(len, data)
	local a = ffi.new( ByteArray, len ) 
	a.count = len
	if data then
		C.ObxFfi_initByteArray(a,data)
	end
	return a
end
function module.createShortArray(len)
	local a = ffi.new( ShortArray, len ) 
	a.count = len
	return a
end
function module.createIntArray(len)
	local a = ffi.new( IntArray, len ) 
	a.count = len
	return a
end
function module.createLongArray(len)
	local a = ffi.new( LongArray, len ) 
	a.count = len
	return a
end
function module.createFloatArray(len)
	local a = ffi.new( FloatArray, len ) 
	a.count = len
	return a
end
function module.createDoubleArray(len)
	local a = ffi.new( DoubleArray, len ) 
	a.count = len
	return a
end
function module.createLuaArray(len)
	local a = { count = len }
	return a
end
local function addElemToSet( set, elem )
	set = bit.bor( set, bit.lshift( 1, elem ) )
	return set
end
function module.addRangeToSet( set, from, to )
	if from > to then
		local tmp = from
		from = to
		to = tmp
	end
	for i=from,to do
		set = addElemToSet(set,i)
	end
	return set
end
local function strlen( str )
	for i=0,str.count-1 do
		if str.data[i] == 0 then
			return i
		end
	end
	return str.count
end
function module.joinStrings( lhs, rhs, wide )
	local lhslen = strlen(lhs)
	local rhslen = strlen(rhs)
	local count = lhslen + rhslen + 1
	local res
	if wide then
		res = ffi.new( WordArray, count )
	else
		res = ffi.new( ByteArray, count )
	end
	local i
	for i = 0,lhslen-1 do
		res.data[i] = lhs.data[i]
	end
	for i = 0,rhslen-1 do
		res.data[i+lhslen] = rhs.data[i]
	end
	res.data[lhslen+rhslen] = 0
	return res
end
function module.charToString(ch,forceWide)
	local a 
	if ch > 255 or forceWide then
		a = ffi.new( WordArray, 2 )
	else
		a = ffi.new( ByteArray, 2 ) 
	end
	a.count = 2
	a.data[0] = ch
	a.data[1] = 0
	return a
end
local function toWide(str)
	local res = ffi.new( WordArray, str.count )
	for i=0,str.count do
		res.data[i] = str.data[i]
	end
	return res
end
function module.stringRelOp( lhs, wideL, rhs, wideR, op )
	if wideL or wideR then
		if not wideL then
			lhs = toWide(lhs)
		end
		if not wideR then
			rhs = toWide(rhs)
		end
		return C.ObxFfi_wstrRelOp(lhs,rhs,op) ~= 0
	else
		return C.ObxFfi_strRelOp(lhs,rhs,op) ~= 0
	end	
end
function module.setSub( lhs, rhs )
	rhs = bit.bnot(rhs)
	return bit.band( lhs, rhs )
end
function module.setDiv( lhs, rhs )
	local tmp1 = bit.bnot( bit.band( lhs, rhs ) )
	local tmp2 = bit.bor( lhs, rhs )
	return bit.band( tmp1, tmp2 )
end
function module.setTest( elem, set )
	return bit.band( set, bit.lshift( 1, elem ) ) ~= 0
end

-- Magic mumbers used by the compiler
module[1] = module.charToStringArray
module[2] = module.createWcharArray
module[3] = module.createShortArray
module[4] = module.createIntArray
module[5] = module.createLongArray
module[6] = module.createFloatArray
module[7] = module.createDoubleArray
module[8] = module.createByteArray
module[9] = addElemToSet
module[10] = module.addRangeToSet
module[11] = bit.bnot
module[12] = bit.bor
module[13] = module.joinStrings
module[14] = C.ObxFfi_DIV
module[15] = C.ObxFfi_MOD
module[16] = module.charToString
module[17] = module.stringRelOp
module[18] = module.setSub
module[19] = bit.band
module[20] = module.setDiv
module[21] = module.setTest

return module

