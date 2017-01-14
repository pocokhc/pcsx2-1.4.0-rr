
local function sleepOne()
  local start = os.time()
  while os.time() - start < 1 do end
end 

local m = memory.readbyte();
print("memory:" .. m);

for i=1,100 do
	print("frame" .. i);
	--sleepOne();
	emu.frameadvance();
end
