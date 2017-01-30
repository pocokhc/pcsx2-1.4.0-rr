
local function sleepOne()
  local start = os.time()
  while os.time() - start < 1 do end
end 


for i=1,10 do
	local m = memory.readbyte(0x480000);
	print("frame" .. i .. " " .. m);
	--sleepOne();
	emu.frameadvance();
end
