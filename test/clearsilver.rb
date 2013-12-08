assert("HDF#set, HDF#get") do
  hdf = ClearSilver::HDF.new
  hdf.set_value "foo", "bar"
  hdf.get_value("foo", "") == "bar"
end

