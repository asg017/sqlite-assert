require "version"

module SqliteAssert
  class Error < StandardError; end
  def self.loadable_path
    File.expand_path('../assert0', __FILE__)
  end
  def self.load(db)
    db.load_extension(self.loadable_path)
  end
end
