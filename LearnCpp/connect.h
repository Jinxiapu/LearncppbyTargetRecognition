#ifndef  _CONNECT_H
#define _CONNECT_H

#include <vector>
#include <algorithm>
#include <Windows.h>

#define SAME(a, b) ((a) == (b))

namespace im {
	typedef int Tlabel;
	typedef BYTE Tin;

	class ConnectedComponents
	{
	public:
		ConnectedComponents(int soft_maxlabels) : labels(soft_maxlabels) {
			clear();
		}
		void clear() {
			std::fill(labels.begin(), labels.end(), Similarity());
			highest_label = 0;
		}
		int connected(const Tin *img, Tlabel *out, int width, int height, bool K8_connectivity);



	private:
		struct Similarity {
			Similarity() : id(0), sameas(0) {}
			Similarity(int _id, int _sameas) : id(_id), sameas(_sameas) {}
			Similarity(int _id) : id(_id), sameas(_id) {}
			int id, sameas, tag;
		};

		bool is_root_label(int id) {
			return (labels[id].sameas == id);
		}
		int root_of(int id) {
			while (!is_root_label(id)) {
				// link this node to its parent's parent, just to shorten
				// the tree.
				labels[id].sameas = labels[labels[id].sameas].sameas;

				id = labels[id].sameas;
			}
			return id;
		}
		bool is_equivalent(int id, int as) {
			return (root_of(id) == root_of(as));
		}
		bool merge(int id1, int id2) {
			if (!is_equivalent(id1, id2)) {
				labels[root_of(id1)].sameas = root_of(id2);
				return false;
			}
			return true;
		}
		int new_label() {
			if (highest_label + 1 > labels.size())
				labels.reserve(highest_label * 2);
			labels.resize(highest_label + 1);
			labels[highest_label] = Similarity(highest_label);
			return highest_label++;
		}

		void label_image(const Tin *img, Tlabel *out, int width, int height,const bool K8_connectivity);

		int relabel_image(Tlabel *out, int width, int height);

		std::vector<Similarity> labels;
		unsigned int highest_label;
	};
}

#endif //  CONNECT_H