#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include <math.h>
#include <algorithm> 

long long BVH::num_nodes = 0;
long long BVH::num_leaves = 0;
long long BVH::num_rays = 0;
long long BVH::num_ray_box = 0;
long long BVH::num_ray_tri = 0;

BVH::~BVH()
{
	delete m_bbox;

	for (int i = 0; i < m_children.size(); i++) {
		delete m_children[i];
	}
}

void
BVH::build(Objects * objs)
{
	if (objs == NULL) {
		std::cerr << "(BVH::build) objs is nullptr" << std::endl;
		return;
	}

    // construct the bounding volume hierarchy
    //m_objects = objs;
	int num = objs->size();
	//Bbox *bboxes = new Bbox[num];
	std::vector<Bbox*> bboxes;
	m_bbox = new Bbox();			// Only for root

	for (int i = 0; i < num; i++) {
		bboxes.push_back(new Bbox());
		//bboxes[i] = Bbox();
		bboxes[i]->addObject((*objs)[i]);
		m_bbox->addObject((*objs)[i]);
	}

	build(bboxes);
}

void
BVH::build(std::vector<Bbox*>& bboxes)
{
	num_nodes++;
	int num = bboxes.size();

	if (num == 0) {
		std::cerr << "(BVH::build) bboxes has size 0" << std::endl;
		return;
	}

	// Make leaf node if only one object
	if (num <= NUM_TRI_IN_LEAF) {
		num_leaves++;
		//m_bbox = bboxes[0];
		m_bbox = new Bbox();

		for (int i = 0; i < num; i++) {
			m_bbox->addObject(bboxes[i]->object[0]);
		}

		for (int i = 0; i < num; i++) {
			delete bboxes[i];
		}
		return;
	}

	// Get split axis based on objects' center
	BVHHelper splits[NUM_SPLITS];
	/*
	int best_plane;
	float minVal, maxVal;
	int axis = getSplitPlane(bboxes, minVal, maxVal);
	float splitLength = (maxVal - minVal) / ((float)NUM_SPLITS);
	*/

	
	Vector3 best_minVal, best_maxVal;
	int best_axis = -1;
	float best_min_cost = MIRO_TMAX;
	int best_plane = -1;
	float best_splitLength;
	
	for (int axis = 0; axis < 3; axis++) {
		Vector3 minVal, maxVal;
		splitPlaneByAxis(bboxes, axis, minVal, maxVal);
		float splitLength = (maxVal[axis] - minVal[axis]) / ((float)NUM_SPLITS);
		int tmp_best_plane = -1;
	
		// initialize
		for (int i = 0; i < NUM_SPLITS; i++) {
			splits[i] = BVHHelper();
		}

		// Put bbox (objects) in the right box split
		for (int i = 0; i < num; i++) {
			if (bboxes[i]->object.size() > 1) {
				std::cerr << "(BVH::build) bboxes[i]->object has size > 1" << std::endl;
				return;
			}
			float boxSplitVal = bboxes[i]->object[0]->center()[axis];
			int index = getSplitIndex(boxSplitVal, minVal[axis], splitLength, axis);

			if (index >= NUM_SPLITS) {
				//if (tmp == NUM_SPLITS) {
				//	index--;				// Include edges to last box split
				//}
				//else {
				std::cerr << "(BVH::build) index > NUM_SPLITS (1)" << std::endl;
				//}
			}
			splits[index].update(bboxes[i]);
		}


		float l = (maxVal[axis] - minVal[axis]) / ((float)NUM_SPLITS);
		float w = maxVal[(axis + 1) % 3] - minVal[(axis + 1) % 3];
		float h = maxVal[(axis + 2) % 3] - minVal[(axis + 2) % 3];

		//// Calculate cost for each box split
		//for (int i = 0; i < NUM_SPLITS; i++) {
		//	// (Not sure if sa is left to 0.0f)
		//	if (splits[i].num_objs == 0) continue;

		//	/*
		//	// Not sure on cost equation
		//	// 2LW + 2(L+W)H
		//	float l = splits[i].max.x - splits[i].min.x;
		//	float w = splits[i].max.y - splits[i].min.y;
		//	float h = splits[i].max.z - splits[i].min.z;
		//	float sa = 2.0f * l * w + (2 * (l + w) * h);
		//	*/
		//	splits[i].sa = sa;
		//}


		// Get the plane with minimum cost
		float min_cost = MIRO_TMAX;

		for (int i = 1; i < NUM_SPLITS; i++) {
			float l_sa = 0.0f, r_sa = 0.0f;
			int l_num = 0, r_num = 0;
			float left_side = 0.0f, right_side = 0.0f;

			// left side
			for (int j = 0; j < i; j++) {
				//l_sa += splits[j].sa;
				l_num += splits[j].num_objs;
			}

			// right side
			for (int j = i; j < NUM_SPLITS; j++) {
				//r_sa += splits[j].sa;
				r_num += splits[j].num_objs;
			}

			//left_side = l_sa * l_num;
			//right_side = r_sa * r_num;

			l_sa = l * i;
			r_sa = l * (NUM_SPLITS - i);
			left_side = 2.0f * l_sa * w + (2 * (l_sa + w) * h);
			right_side = 2.0f * r_sa * w + (2 * (r_sa + w) * h);

			float cost = left_side * l_num + right_side * r_num;

			if (cost < min_cost) {
				min_cost = cost;
				tmp_best_plane = i;
			}
		}
	
		if (min_cost < best_min_cost) {
			best_min_cost = min_cost;
			best_plane = tmp_best_plane;
			best_axis = axis;
			best_minVal = minVal;
			best_maxVal = maxVal;
			best_splitLength = splitLength;
		}
	}

	// Put bbox (objects) in the left or right split
	std::vector<Bbox*> leftBoxes; //= new Bbox[best_l_num];
	std::vector<Bbox*> rightBoxes; //= new Bbox[best_r_num];
	Bbox * lBox = new Bbox();
	Bbox * rBox = new Bbox();

	int l_index = 0, r_index = 0;

	for (int i = 0; i < num; i++) {
		float boxSplitVal = bboxes[i]->object[0]->center()[best_axis];
		int index = getSplitIndex(boxSplitVal, best_minVal[best_axis], best_splitLength, best_axis);
		float boundary = m_bbox->min[best_axis] + (best_plane * best_splitLength);

		//float boxSplitVal = bboxes[i]->object[0]->center()[axis];
		//int index = getSplitIndex(boxSplitVal, minVal, splitLength, axis);
		//float boundary = m_bbox->min[axis] + (best_plane * splitLength);

		if (index < best_plane) {
			//leftBoxes[l_index++] = *bboxes[i];
			leftBoxes.push_back(bboxes[i]);
			lBox->addObject(bboxes[i]->object[0]);
		}
		else {
			//rightBoxes[r_index++] = *bboxes[i];
			rightBoxes.push_back(bboxes[i]);
			rBox->addObject(bboxes[i]->object[0]);
		}
	}

	// delete ptrs
	//delete splits;

	BVH *leftChild = new BVH();
	BVH *rightChild = new BVH();

	leftChild->m_bbox = lBox;
	rightChild->m_bbox = rBox;

	leftChild->build(leftBoxes);
	rightChild->build(rightBoxes);

	m_children.push_back(leftChild);
	m_children.push_back(rightChild);
}

int BVH::getSplitPlane(std::vector<Bbox*>& bboxes, float &minVal, float &maxVal)
{
	Vector3 min = Vector3(MIRO_TMAX, MIRO_TMAX, MIRO_TMAX);
	Vector3 max = Vector3(-MIRO_TMAX, -MIRO_TMAX, -MIRO_TMAX);

	for (int i = 0; i < bboxes.size(); i++) {
		Object* obj = bboxes[i]->object[0];
		Vector3 pos = obj->center();

		min.x = std::min(min.x, pos.x);
		min.y = std::min(min.y, pos.y);
		min.z = std::min(min.z, pos.z);

		max.x = std::max(max.x, pos.x);
		max.y = std::max(max.y, pos.y);
		max.z = std::max(max.z, pos.z);
	}

	Vector3 diff = max - min;

	if ((diff.x > diff.y) && (diff.x > diff.z)) {
		minVal = min.x;
		maxVal = max.x;
		return X_AXIS;
	}
	else if (diff.y > diff.z) {
		minVal = min.y;
		maxVal = max.y;
		return Y_AXIS;
	}
	else {
		minVal = min.z;
		maxVal = max.z;
		return Z_AXIS;
	}



	/*Vector3 diff = m_bbox->max - m_bbox->min;

	if ((diff.x > diff.y) && (diff.x > diff.z)) return X_AXIS;
	else if (diff.y > diff.z)					return Y_AXIS;
	else										return Z_AXIS;*/

}


void BVH::splitPlaneByAxis(std::vector<Bbox*>& bboxes, int axis, Vector3 &minVal, Vector3 &maxVal)
{
	minVal = Vector3(MIRO_TMAX, MIRO_TMAX, MIRO_TMAX);
	maxVal = Vector3(-MIRO_TMAX, -MIRO_TMAX, -MIRO_TMAX);

	for (int i = 0; i < bboxes.size(); i++) {
		Object* obj = bboxes[i]->object[0];
		Vector3 pos = obj->center();

		minVal.x = std::min(minVal.x, pos.x);
		minVal.y = std::min(minVal.y, pos.y);
		minVal.z = std::min(minVal.z, pos.z);

		maxVal.x = std::max(maxVal.x, pos.x);
		maxVal.y = std::max(maxVal.y, pos.y);
		maxVal.z = std::max(maxVal.z, pos.z);
	}
}

int BVH::getSplitIndex(float val, float minVal, float splitLength, int axis)
{
	//float tmp = (boxSplitVal - m_bbox->min[axis]) / splitLength;
	//int index = floor(tmp);
	//int index = fmod((boxSplitVal - m_bbox->min[axis]), splitLength);
	int index = -1;
	float boundary = minVal;

	for (int i = 0; i < NUM_SPLITS; i++) {
		if (val < boundary) {
			break;
		}
		index = i;
		boundary += splitLength;
	}

	if (index == -1) {
		std::cerr << "(BVH::getSplitIndex) index is -1" << std::endl;
	}

	return index;
}

bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    bool hit = false;
    HitInfo tempMinHit;
    
	for (size_t i = 0; i < m_bbox->object.size(); ++i)
    {
		//if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
		if (m_bbox->object[i]->intersect(tempMinHit, ray, tMin, tMax))
		{
			num_ray_tri++;
            hit = true;
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
		num_rays++;
    }
    
    return hit;
}

bool BVH::intersect_bvh(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	bool is_leaf = (m_children.size() == 0);

	num_rays++;
	if (m_bbox->hit(ray)) {
		BVH::num_ray_box++;

		if (is_leaf) {
			return intersect(minHit, ray, tMin, tMax);
		}
		// for each child box
		else {
			// Error check
			if (m_children.size() == 0) {
				std::cerr << "(BVH::intersect_bvh) m_children is empty" << std::endl;
				return false;
			}

			bool didHit = false;
			/*bool hitLeft, hitRight;
			float l_tMin, l_tMax, r_tMin, r_tMax;

			hitLeft = m_children[0]->m_bbox->hit(ray, l_tMin, l_tMax);
			hitRight = m_children[1]->m_bbox->hit(ray, r_tMin, r_tMax);
			
			// Only hits left child
			if (hitLeft && !hitRight) {
				return m_children[0]->intersect_bvh(minHit, ray, tMin, tMax);
			}
			// Only hits right child
			else if (!hitLeft && hitRight) {
				return m_children[1]->intersect_bvh(minHit, ray, tMin, tMax);
			}
			// Hits both left and right children
			else if (hitLeft && hitRight) {
				// Not sure if comparing the right values
				if (l_tMin <= r_tMin) {
					didHit |= m_children[0]->intersect_bvh(minHit, ray, tMin, tMax);
					didHit |= m_children[1]->intersect_bvh(minHit, ray, tMin, tMax);
				}
				else {
					didHit |= m_children[1]->intersect_bvh(minHit, ray, tMin, tMax);
					didHit |= m_children[0]->intersect_bvh(minHit, ray, tMin, tMax);
				}
			}
			*/
			
			

			// Check left
			if (m_children[0]->intersect_bvh(minHit, ray, tMin, tMax))
				didHit = true;
			// Check right
			if (m_children[1]->intersect_bvh(minHit, ray, tMin, tMax))
				didHit = true;

			return didHit;
		}
	}
	return false;
}

void Bbox::addObject(Object * obj)
{
	if (obj == NULL) {
		std::cerr << "(Bbox::addObject) obj is nullptr" << std::endl;
		return;
	}

	object.push_back(obj);

	min.x = std::min(min.x, obj->min().x);
	min.y = std::min(min.y, obj->min().y);
	min.z = std::min(min.z, obj->min().z);

	max.x = std::max(max.x, obj->max().x);
	max.y = std::max(max.y, obj->max().y);
	max.z = std::max(max.z, obj->max().z);
}

bool Bbox::hit(const Ray& ray)
{
	float tMin, tMax;
	return hit(ray, tMin, tMax);
}

bool Bbox::hit(const Ray& ray, float& tMin, float& tMax)
{
	Vector3 t1, t2;

	// http://www.cs.utah.edu/~awilliam/box/box.pdf
	/*float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if (ray.d.x >= 0.0f) {
		tmin = (min.x - ray.o.x) / ray.d.x;
		tmax = (max.x - ray.o.x) / ray.d.x;
	}
	else {
		tmin = (max.x - ray.o.x) / ray.d.x;
		tmax = (min.x - ray.o.x) / ray.d.x;
	}
	if (ray.d.y >= 0.0f) {
		tymin = (min.y - ray.o.y) / ray.d.y;
		tymax = (max.y - ray.o.y) / ray.d.y;
	}
	else {
		tymin = (max.y - ray.o.y) / ray.d.y;
		tymax = (min.y - ray.o.y) / ray.d.y;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	if (ray.d.z >= 0.0f) {
		tzmin = (min.z - ray.o.z) / ray.d.z;
		tzmax = (max.z - ray.o.z) / ray.d.z;
	}
	else {
		tzmin = (max.z - ray.o.z) / ray.d.z;
		tzmax = (min.z - ray.o.z) / ray.d.z;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	
	return true;
	*/

	t1 = hitHelper(ray, min);
	t2 = hitHelper(ray, max);

	tMin = std::max(std::max(std::min(t1.x, t2.x), std::min(t1.y, t2.y)), std::min(t1.z, t2.z));
	tMax = std::min(std::min(std::max(t1.x, t2.x), std::max(t1.y, t2.y)), std::max(t1.z, t2.z));


	return //(tMax < 0.0f) ||
		(tMin <= tMax);
}

Vector3 Bbox::hitHelper(const Ray& ray, const Vector3& v)
{
	Vector3 t;

	t.x = (v.x - ray.o.x) / ray.d.x;
	t.y = (v.y - ray.o.y) / ray.d.y;
	t.z = (v.z - ray.o.z) / ray.d.z;

	return t;
}

void BVHHelper::update(Bbox *box)
{
	if (box == NULL) {
		std::cerr << "(BVHHelper::update) box is nullptr" << std::endl;
		return;
	}

	num_objs++;

	min.x = std::min(min.x, box->min.x);
	min.y = std::min(min.y, box->min.y);
	min.z = std::min(min.z, box->min.z);

	max.x = std::max(max.x, box->max.x);
	max.y = std::max(max.y, box->max.y);
	max.z = std::max(max.z, box->max.z);
}