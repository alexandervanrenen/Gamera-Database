# Some helper methods for serializing json
# -----------------------------------------------
module JsonHelper
  # Serializes an array of strings
  def self.serializeArray(data)
    res = "["
    for index in (0...data.size-1) do
      res += "\"#{data[index]}\""
      res += ","
    end
    res += "\"#{data[data.size-1]}\"" if data.size != 0
    res += "]"
  end

  # Serializes an array of arrays of strings
  def self.serializeArrayOfArrays(data)
    res = "["
    for index in (0...data.size-1) do
      res += serializeArray(data[index]) + ","
    end
    res += serializeArray(data[data.size-1]) if data.size != 0
    res += "]"
  end
end
# -----------------------------------------------
