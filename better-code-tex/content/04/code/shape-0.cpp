class shape {
  public:
    virtual ~shape() { }
    virtual void draw(ostream&, size_t) const = 0;
};

using document = vector<shared_ptr<shape>>;

void draw(const document& x, ostream& out, size_t position)
{
    out << string(position, ' ') << "<document>" << endl;
    for (const auto& e: x) e->draw(out, position + 2);
    out << string(position, ' ') << "</document>" << endl;
}