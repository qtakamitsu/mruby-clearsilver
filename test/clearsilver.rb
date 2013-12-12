assert("HDF#set, HDF#get") do
  hdf = ClearSilver::HDF.new
  hdf.set_value "foo", "bar"
  hdf.get_value("foo", "") == "bar"
end

assert("CS#parse_string") do
  hdf = ClearSilver::HDF.new
  hdf.set_value "name", "Administrator"

  cs = ClearSilver::CS.new(hdf)
  cs.parse_string('Welcom to ClearSilver, <?cs var:name ?>!')
  cs.render() == "Welcom to ClearSilver, Administrator!"
end

